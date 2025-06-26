#pragma once

#include <emscripten.h>
#include <emscripten/bind.h>

#include <stdint.h>

#include "../../src/database/bibfile.hpp"
#include "../../src/database/parser.hpp"
#include "../../src/recipes/fieldfilter.hpp"
#include "../../src/uni/fieldnormalization.hpp"
#include "../../src/database/printer.hpp"

#include "../handlers/jsapihandler.hpp"

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
        .smart_ptr_constructor("BibDB", &std::make_shared<BibDB>)
        .function("clear", &BibDB::clear)
        .function("cleanup", &BibDB::cleanup)
        .function("countKeep", &BibDB::countKeep)
        .property("numEntries", &BibDB::numEntries)
        .property("numSpecialEntries", &BibDB::numSpecialEntries)
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

    class_<FieldNormalizer>("FieldNormalizer")
        .constructor<std::shared_ptr<BibDB>>()
        .function("addUTF8Preamble", &FieldNormalizer::addUTF8Preamble)
        .function("NFCNormalize", &FieldNormalizer::NFCNormalize)
        .function("NFKCNormalize", &FieldNormalizer::NFKCNormalize)
        .function("uni2latex", &FieldNormalizer::uni2latex)
        ;

    class_<Printer>("Printer")
        .constructor<std::shared_ptr<BibDB>>()
        .function("toBibFile", &Printer::toBibFile)
        .function("toString", &Printer::toString)
        .function("toEntriesAndKeys", &Printer::toEntriesAndKeys)
        ;

    class_<JsApiHandler>("JsApiHandler")
        .constructor<std::shared_ptr<BibDB>>()
        .function("getPrepArXiv", &JsApiHandler::getPrepArXiv)
        .function("getPrepDOI", &JsApiHandler::getPrepDOI)
        .function("getArXivIds", &JsApiHandler::getArXivIds)
        .function("getDOIs", &JsApiHandler::getDOIs)
        .function("getBibKeysFromIndex", &JsApiHandler::getBibKeysFromIndex)
        .function("setUnkeepFromIndex", &JsApiHandler::setUnkeepFromIndex)
        .function("updateArXivFromResponseAndGetDOIs", &JsApiHandler::updateArXivFromResponseAndGetDOIs)
        .function("updateDOIFromResponse", &JsApiHandler::updateDOIFromResponse)
        .function("addFromArXivResponseAndGetDoi", &JsApiHandler::addFromArXivResponseAndGetDoi)
        .function("addFromDOIResponse", &JsApiHandler::addFromDOIResponse)
        ;

    register_vector<uint64_t>("Uint64Vector");
    register_vector<size_t>("SizeTVector");
    register_vector<std::string>("StringVector");
    register_map<uint64_t, std::string>("Uint64StringMap");
    register_map<std::string, std::string>("StringStringMap");
}

