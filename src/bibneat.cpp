#include "database/parser.hpp"
#include "database/printer.hpp"
#include "recipes/fieldfilter.hpp"
#include "recipes/apicalls.hpp"
#include "uni/fieldnormalization.hpp"
#include "cli/cliParser.hpp"
#include "utils/exitCodes.hpp"
#include "utils/attributes.hpp"

#include <stdint.h>
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

    CliParser cli("bibneat");
    cli.usageString = " [options] -i input1.bib -i input2.bib ... -o output.bib";

    // help
    cli.makeCategory("HELP");
    cli.addFlag("help", 
        "Displays this message.", 
        false,
        "help", "h");
    cli.addToCategory("HELP","help");
    cli.addFlag("verbose", 
        "Print information on what bibneat is doing.", 
        false,
        "verbose", "v");
    cli.addToCategory("HELP","verbose");
    // IO
    cli.makeCategory("IO");
    cli.addVectorOption<std::string>(true,"input",
        "Input .bib files for bibneat.", 
        "","i");
    cli.addToCategory("IO","input");
    cli.addOption<std::string>(false,"output", 
        "Output .bib file (default bibneat.bib). Can be the same as one of the input files.", 
        "","o",
        "bibneat.bib");
    cli.addToCategory("IO","output");
    cli.addFlag("append", 
        "Append to output file rather than overwriting it.", 
        false, 
        "append", "a");
    cli.addToCategory("IO","append");
    // bibtex
    cli.makeCategory("BibTex");
    cli.addFlag("keep-bibtex", 
        "Keep only fields that BibTex knows about (delete the others).", 
        false, 
        "keep-bibtex", "kb");
    cli.addToCategory("BibTex","keep-bibtex");
    // arXiv API
    cli.makeCategory("ArXiv API");
    cli.addFlag("check-arxiv", 
        "Check the arXiv API for each arXiv entry (validates the arXiv ID and checks whether a related publication exists). Do not alter the entry.", 
        false, 
        "arxiv-check", "xc");
    cli.addToCategory("ArXiv API","check-arxiv");
    cli.addFlag("replace-arxiv", 
        "Replace each arXiv entry with the BibTex received from the arXiv API. Do not follow known DOIs for publications.", 
        false, 
        "arxiv-replace", "xr");
    cli.addToCategory("ArXiv API","replace-arxiv");
    cli.addFlag("follow-arxiv", 
        "Replace each arXiv entry with the BibTex received from the arXiv API. If there is a known publication DOI, replace the arXiv entry with the published version.", 
        false, 
        "arxiv-follow", "xf");
    cli.addToCategory("ArXiv API","follow-arxiv");
    // DOI API
    cli.makeCategory("doi.org API");
    cli.addFlag("check-doi", 
        "Check the doi.org API for each entry with known DOI (validates the DOI). Do not alter the entry.", 
        false, 
        "doi-check", "dc");
    cli.addToCategory("doi.org API","check-doi");
    cli.addFlag("replace-doi", 
        "Replace each entry with known DOI with the BibTex received from the doi.org API.", 
        false, 
        "doi-replace", "dr");
    cli.addToCategory("doi.org API","replace-doi");
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
    GET_FLAG(verbose, "verbose")
    // IO
    GET_VECTOR(pInputs,std::string,"input")
    if(pInputs->size()==0){
        std::cerr << "ERROR: At least one input file is required" <<std::endl << std::endl;
        cli.printUsage();
        return EC_BAD_ARGS;
    }
    GET_VALUE(pOutput,std::string,"output")
    if(pOutput->empty()){
        std::cerr << "ERROR: Empty output file" <<std::endl << std::endl;
        cli.printUsage();
        return EC_BAD_ARGS;
    }
    GET_FLAG(appendOutput, "append")
    // bibtex
    GET_FLAG(keep_bibtex, "keep-bibtex")
    // arXiv API
    GET_FLAG(check_arxiv, "check-arxiv")
    GET_FLAG(replace_arxiv, "replace-arxiv")
    GET_FLAG(follow_arxiv, "follow-arxiv")
    const bool doArxiv = check_arxiv||replace_arxiv||follow_arxiv;
    const bool doReplaceArxiv =replace_arxiv||follow_arxiv;
    const bool doReplaceDOIArxiv = follow_arxiv;
    // DOI API
    GET_FLAG(check_doi, "check-doi")
    GET_FLAG(replace_doi, "replace-doi")
    const bool doDOI = check_doi||replace_doi;
    const bool doReplaceDOI = replace_doi;
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
    Parser parser(bibDB, verbose);

    bool parseSuccess;
    for (auto& input: *pInputs){
        parseSuccess = parser.parse(input);
        if(!parseSuccess){
            std::cerr<< "Error parsing file " << input <<std::endl;
            std::cerr<< parser.currentStr <<std::endl;
            return EC_BAD_PARSE;
        }
    }
    uint64_t numStartingEntries=bibDB->countKeep();

    // do API calls first if we have to
    ApiCaller apiCaller(bibDB, connTimeout, totTimeout, verbose);
    if(doArxiv){
        apiCaller.checkArXiv(doReplaceArxiv, doReplaceDOIArxiv);
    }
    if(doDOI){
        apiCaller.checkDOI(doReplaceDOI);
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

    // Let's check no entries fell off the back of the truck
    if(numStartingEntries!=bibDB->countKeep()){
        std::cerr<< 
        "So... *awkward* I appear to have lost track of some of your bib entries " 
        "(or magicked more out of thin air... tada!). "
        "This should NEVER happen in bibneat, so this is definitively a bug somewhere "
        "(parsing bibtex is very hard, sorry). On the plus side, "
        "I have not touched your .bib files, nor have I produced any output. "
        "This should cause you the least number of headaches possible. "
        "You are welcome. Can we still be friends?"
        <<std::endl;
        return EC_LOST_ENTRIES;
    }

    // done! We can now print the output and be done...
    Printer printer(bibDB);
    bool printSuccess = printer.toBibFile(*pOutput, !appendOutput);

    if(!printSuccess){
        std::cerr<< "printing to output file " << (*pOutput) << " failed" << std::endl;
        return EC_BAD_PRINT;
    }

    return EC_SUCCESS;
}