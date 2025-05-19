#include "database/parser.hpp"
#include "database/printer.hpp"
#include "recipes/fieldfilter.hpp"
#include "recipes/apicalls.hpp"
#include "uni/fieldnormalization.hpp"
#include "cli/cliParser.hpp"
#include "utils/exitCodes.hpp"
#include "utils/attributes.hpp"

#include <iostream>

#define GET_FLAG(NAME,ONAME) \
    auto [NAME, s ## NAME ## _flag]=cli.getFlag(ONAME);\
    if(!(s ## NAME ## _flag)){bailBadParse(&cli);}\

#define GET_VALUE(NAME,OTYPE,ONAME) \
    auto [NAME, s ## NAME ## _flag]=cli.getValue<OTYPE>(ONAME);\
    if(!(s ## NAME ## _flag)){bailBadParse(&cli);}\

#define GET_VECTOR(NAME,OTYPE,ONAME) \
    auto [NAME, s ## NAME ## _flag]=cli.getVector<OTYPE>(ONAME);\
    if(!(s ## NAME ## _flag)){bailBadParse(&cli);}\

INLINE static int bailBadParse(CliParser* cli){
    std::cerr<<"Error parsing cli options" <<std::endl << std::endl;
    cli->printUsage();
    return EC_BAD_PARSE;
}

int main(int argc, char** argv){

    CliParser cli("bibgrab");
    cli.usageString = " [options] -x arXiv id -x arXiv id ...  -d doi -d doi.org/... ... -o output.bib";

    // help
    cli.makeCategory("HELP");
    cli.addFlag("help", 
        "Displays this message.", 
        false,
        "help", "h");
    cli.addToCategory("HELP","help");
    // Inputs
    cli.makeCategory("INPUT");
    cli.addVectorOption<std::string>(true,"arxiv-input",
        "ArXiv entry to look up. Accepts arXiv Ids (with or without the arXiv: prefix) and arxiv.org urls.", 
        "arxiv","x");
    cli.addToCategory("INPUT","arxiv-input");
    cli.addVectorOption<std::string>(true,"doi-input",
        "DOI or doi.org url to look up.", 
        "doi","d");
    cli.addToCategory("INPUT","doi-input");
    cli.addFlag("follow-arxiv", 
        "For each arXiv entry, if there is a known publication DOI, replace the arXiv entry with the published version.", 
        false, 
        "arxiv-follow", "f");
    cli.addToCategory("INPUT","follow-arxiv");
    // IO
    cli.makeCategory("OUTPUT");
    cli.addOption<std::string>(false,"output", 
        "Output .bib file (default bibneat.bib). Can be the same as one of the input files.", 
        "","o",
        "bibneat.bib");
    cli.addToCategory("OUTPUT","output");
    cli.addFlag("append", 
        "Append to output file rather than overwriting it.", 
        false, 
        "append", "a");
    cli.addToCategory("OUTPUT","append");
    cli.addFlag("output-off", 
        "Just perform request, do not write .bib file.", 
        false, 
        "no-output", "no");
    cli.addToCategory("OUTPUT","output-off");
    // bibtex
    cli.makeCategory("BibTex");
    cli.addFlag("keep-bibtex", 
        "Keep only fields that BibTex knows about (delete the others).", 
        false, 
        "keep-bibtex", "kb");
    cli.addToCategory("BibTex","keep-bibtex");
    // CURL Options
    cli.makeCategory("CURL Options");
    cli.addOption<long>(false,"timeout", 
        "Timeout (in seconds) for each API request. Default 20s.", 
        "timeout","t",
        20L);
    cli.addToCategory("CURL Options","timeout");
    cli.addOption<long>(false,"connection-timeout", 
        "Timeout (in seconds) for the CURL connection stage (resolution+handshake) for each API request. Default 15s. Must be > timeout to have effect.", 
        "connection-timeout","",
        15L);
    cli.addToCategory("CURL Options","connection-timeout");
    // Unicode
    cli.makeCategory("Unicode");
    cli.addFlag("add-preamble", 
        "Add UTF8 encoding as BibTex preamble (inputenc and fontenc packages must be present in latex document preamble to have effect).", 
        false, 
        "add-encoding-preamble", "p");
    cli.addToCategory("Unicode","add-preamble");
    cli.addFlag("canon-normalize", 
        "Perform canonical unicode normalization of each entry (may fix compilation errors).", 
        false, 
        "unicode-canon-normalize", "uc");
    cli.addToCategory("Unicode","canon-normalize");
    cli.addFlag("compat-normalize", 
        "Perform compatibility-optimized unicode normalization of each entry (should fix most compilation errors, but may result in loss of details in non-latin character sets).", 
        false, 
        "unicode-compat-normalize", "uk");
    cli.addToCategory("Unicode","compat-normalize");
    cli.addFlag("unicode2latex", 
        "Transforms unicode special characters (accents, symbols, etc.) in their equivalent latex encoding (may fix compilation errors).", 
        false, 
        "unicode2latex", "u2l");
    cli.addToCategory("Unicode","unicode2latex");
    
    if(!cli.parseArgs(argc,argv)){
        cli.printError();
        return EC_BAD_PARSE;
    }

    // Deal with help
    GET_FLAG(help, "help")
    if(help){
        cli.printHelp();
        return EC_SUCCESS;
    }

    // IO
    GET_VECTOR(pXInputs,std::string,"arxiv-input")
    GET_VECTOR(pDInputs,std::string,"doi-input")
    if(pXInputs->size()==0 && pDInputs->size()==0){
        std::cerr << "ERROR: At least one input file is required" <<std::endl << std::endl;
        cli.printUsage();
        return EC_BAD_ARGS;
    }
    GET_VALUE(pOutput,std::string,"output")
    GET_FLAG(noOutput, "output-off")
    if(pOutput->empty()&&(!noOutput)){
        std::cerr << "ERROR: Empty output file" <<std::endl << std::endl;
        cli.printUsage();
        return EC_BAD_ARGS;
    }
    GET_FLAG(appendOutput, "append")
    // arXiv API
    GET_FLAG(follow_arxiv, "follow-arxiv")
    const bool doArxiv = (pXInputs->size()>0);
    const bool doReplaceDOIArxiv = follow_arxiv;
    // DOI API
    const bool doDOI = (pDInputs->size()>0);
    // bibtex
    GET_FLAG(keep_bibtex, "keep-bibtex")
    // CURL Options
    GET_VALUE(pTotTimeout,long,"timeout")
    long totTimeout = *pTotTimeout;
    GET_VALUE(pConnTimeout,long,"connection-timeout")
    long connTimeout = *pConnTimeout;
    // Unicode
    GET_FLAG(add_preamble, "add-preamble")
    GET_FLAG(canon_normalize, "canon-normalize")
    GET_FLAG(compat_normalize, "compat-normalize")
    GET_FLAG(uni2latex, "unicode2latex")

    // OK we can begin 
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();

    bool parseSuccess;
    ApiCaller apiCaller(bibDB, connTimeout, totTimeout);
    if(doArxiv){
        for(auto& arXivInput: *pXInputs){
            parseSuccess = apiCaller.getArXivImmediate(arXivInput,doReplaceDOIArxiv);
            if(!parseSuccess){return EC_BAD_PARSE;}
        }
    }
    if(doDOI){
        for(auto& doiInput: *pDInputs){
            parseSuccess = apiCaller.getDOIImmediate(doiInput);
            if(!parseSuccess){return EC_BAD_PARSE;}
        }
    }

    // Now we can filter the fields. Entries will no longer be added
    FieldFilter fieldfilter(bibDB);
    if(keep_bibtex){
        fieldfilter.keepBibTex();
    }

    // now Unicode stuff
    FieldNormalizer normalizer(bibDB);
    if(add_preamble){
        normalizer.addUTF8Preamble();
    }
    if(compat_normalize){
        normalizer.NFKCNormalize();
    } else if (canon_normalize){
        normalizer.NFCNormalize();
    }
    if(uni2latex){
        normalizer.uni2latex();
    }

    // done! We can now print the output and be done...
    Printer printer(bibDB);
    if(!noOutput){
        bool printSuccess = printer.toBibFile(*pOutput, !appendOutput);
        if(!printSuccess){
            std::cerr<< "printing to output file " << (*pOutput) << " failed" << std::endl;
            return EC_BAD_PRINT;
        }
    }

    return EC_SUCCESS;
}