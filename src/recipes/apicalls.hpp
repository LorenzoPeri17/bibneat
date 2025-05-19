#pragma once

#include <string>
#include <regex>
#include <iostream>

#include <stdint.h>

#include "../utils/umap.hpp"
#include "../utils/stringUtils.hpp"
#include "../utils/debug.hpp"
#include "../utils/attributes.hpp"

#include "../database/bibfile.hpp"
#include "../database/parser.hpp"

#include "../requests/requests.h"

class ApiCaller{
public:

    explicit ApiCaller(
        std::shared_ptr<BibDB> bibDB,
        long connTimeout, 
        long totTimeout,
        const bool verbose = false
        ): 
            connTimeout(connTimeout),
            totTimeout(totTimeout),
            bibDB(bibDB),
            verbose(verbose),
            shadowDB(std::make_shared<BibDB>()),
            shadowParser(this->shadowDB) {
            this->CapiBib = std::make_unique<char[]>(this->N_RESERVE_API_BIB);
        }

    virtual ~ApiCaller(){}

    void checkArXiv(const bool replaceWithArxiv, const bool replaceWithDoi) noexcept;
    bool getArXivImmediate(const std::string arxiv, const bool replaceWithDoi) noexcept;

    void checkDOI(const bool replaceWithDoi) noexcept;
    bool getDOIImmediate(const std::string doi) noexcept;

    long connTimeout;
    long totTimeout;

protected: // protected for testing
    const std::string getArXivId(const BibEntry* entry) noexcept;
    void checkArXivEntry(BibEntry* entry, const bool replaceWithArxiv, const bool replaceWithDoi) noexcept;
    const std::string prepArXiv(const std::string arxiv) noexcept;
    
    bool hasDOI(const BibEntry* entry) const noexcept;
    const std::string prepDOI(const std::string doi) noexcept;
    void checkDOIEntry(BibEntry* entry, const bool replaceWithDoi, const std::string doi, const bool useString) noexcept;

private:
    std::shared_ptr<BibDB> bibDB;
    const uint32_t N_RESERVE_API_BIB = 2<<20;
    std::unique_ptr<char[]> CapiBib;
    std::string apiBib;
    uint32_t NapiBib;

    const bool verbose;

    std::shared_ptr<BibDB> shadowDB;
    Parser shadowParser;
    const uint32_t N_REQUEST_BEFORE_CLEAN = 50;

    bool getFromArXivApi(const BibEntry* entry) noexcept;
    bool getFromDOIApi(const BibEntry* entry) noexcept;
    bool getFromDOIApi(const std::string doi) noexcept;
    RequestsCode RCgetFromDOIApi(const std::string doi) noexcept;

    const std::string arXivIdPattStr = "((?:\\d{4}\\.\\d{4,5})|(?:[\\w-]+(?:\\.[A-Z]{2})?\\/\\d{7}))";

    const std::regex arXivUrlPattern=std::regex(
        "(?:https?:\\/\\/)?arxiv\\.org\\/(?:(?:abs)|(?:pdf))\\/" + arXivIdPattStr, 
        std::regex::icase|std::regex::ECMAScript|std::regex::optimize
    );

    const std::regex arXivDOIPattern=std::regex(
        "ar(?:x|X)iv\\." + arXivIdPattStr, 
        std::regex::icase|std::regex::ECMAScript|std::regex::optimize
    );

    const std::regex arXivNotePattern=std::regex(
        "ar(?:x|X)iv:" + arXivIdPattStr, 
        std::regex::icase|std::regex::ECMAScript|std::regex::optimize
    );

    const std::regex arXivIdPattern=std::regex(
        arXivIdPattStr, 
        std::regex::icase|std::regex::ECMAScript|std::regex::optimize
    );

    const std::regex arXivImmediatePattern=std::regex(
        "(?:(?:(?:https?:\\/\\/)?arxiv\\.org\\/(?:[^ \\n\\/]+)\\/)|(?:ar(?:x|X)iv[^ \\n]))?"+arXivIdPattStr, 
        std::regex::icase|std::regex::ECMAScript|std::regex::optimize
    );

    const std::regex DOIPattern=std::regex(
        "(?:https?:\\/\\/(?:dx\\.)?doi\\.org\\/)?(10.\\d{4,}\\/[^ \\n]+)", 
        std::regex::icase|std::regex::ECMAScript|std::regex::optimize
    );

};