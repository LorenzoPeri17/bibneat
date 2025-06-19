#include <emscripten.h>
#include <emscripten/bind.h>

#include "../../src/database/bibfile.hpp"
#include "../../src/database/parser.hpp"
#include "../../src/recipes/fieldfilter.hpp"
#include "../../src/recipes/apicalls.hpp"
#include "../../src/uni/fieldnormalization.hpp"
#include "../../src/database/printer.hpp"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(bibneat_module) {
    class_<BibEntry>("BibEntry")
        .constructor<std::string, std::string>()
        .function("addAttribute", &BibEntry::addAttribute)
        .function("cleanup", &BibEntry::cleanup)
        .function("findBibType", &BibEntry::findBibType)
        .property("bibKey", &BibEntry::bibKey)
        .property("bibTypeName", &BibEntry::bibTypeName)
        .property("attributes", &BibEntry::attributes)
        .property("keep", &BibEntry::keep)
        .property("doiChecked", &BibEntry::doiChecked)
        .property("arxivChecked", &BibEntry::arxivChecked)
        .property("bibType", &BibEntry::bibType)
        ;

    class_<SpecialEntry>("SpecialEntry")
        .constructor<std::string, std::string>()
        .property("bibTypeName", &SpecialEntry::bibTypeName)
        .property("body", &SpecialEntry::body)
        ;

    class_<BibDB>("BibDB")
        .constructor<>()
        .function("countKeep", &BibDB::countKeep)
        ;

    class_<Parser>("Parser")
        .constructor<std::shared_ptr<BibDB>, const bool>()
        .function("parse", &Parser::parse)
        .function("parseString", &Parser::parseString)
        ;

    class_<FieldFilter>("FieldFilter")
        .constructor<std::shared_ptr<BibDB>>()
        .function("keepBibTex", &FieldFilter::keepBibTex)
        ;

    class_<ApiCaller>("ApiCaller")
        .constructor<std::shared_ptr<BibDB>, long, long, const bool>()
        .function("checkArXiv", &ApiCaller::checkArXiv)
        .function("getArXivImmediate", &ApiCaller::getArXivImmediate)
        .function("checkDOI", &ApiCaller::checkDOI)
        .function("getDOIImmediate", &ApiCaller::getDOIImmediate)
        .property("connTimeout", &ApiCaller::connTimeout)
        .property("totTimeout", &ApiCaller::totTimeout)
        ;

    class_<FieldNormalizer>("FieldNormalizer")
        .constructor<std::shared_ptr<BibDB>>()
        .function("addUTF8Preamble", &FieldNormalizer::addUTF8Preamble)
        .function("NFCNormalize", &FieldNormalizer::NFCNormalize)
        .function("NFKCNormalize", &FieldNormalizer::NFKCNormalize)
        .function("uni2latex", &FieldNormalizer::uni2latex)
        .property("UError", &FieldNormalizer::UError)
        ;

    class_<Printer>("Printer")
        .constructor<std::shared_ptr<BibDB>>()
        .function("toBibFile", &Printer::toBibFile)
        ;
}

