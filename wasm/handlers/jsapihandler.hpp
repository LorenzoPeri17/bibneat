#pragma once

#include <map>
#include <memory>
#include <string>

#include <stdint.h>

#include "../../src/database/bibfile.hpp"
#include "../../src/database/parser.hpp"
#include "../../src/recipes/apicalls.hpp"
#include "../../src/utils/umap.hpp"
#include "../../src/utils/attributes.hpp"

class JsApiHandler: protected ApiCaller {
public:
    explicit JsApiHandler (
        std::shared_ptr<BibDB> bibDB
        // we have no plan of making requests
        ): ApiCaller(bibDB, 0L, 0L),
           directParser(bibDB)
        {}

    std::string getPrepArXiv(std::string arxiv){
        return this->prepArXiv(arxiv);
    }

    std::string getPrepDOI(std::string doi){
        return this->prepDOI(doi);
    }

    std::map<uint64_t,std::string> getArXivIds();
    std::map<uint64_t,std::string> getDOIs();

    std::string getBibKeysFromIndex(uint64_t index);
    void setUnkeepFromIndex(uint64_t index);

    std::map<uint64_t,std::string> updateArXivFromResponseAndGetDOIs(std::vector<uint64_t> indices, std::vector<std::string> bibTex, const bool doReplace);
    void updateDOIFromResponse(std::vector<uint64_t> indices, std::vector<std::string> bibTex);

    std::map<uint64_t,std::string> addFromArXivResponseAndGetDoi(std::string res);
    void addFromDOIResponse(std::string res);

private:
    std::map<uint64_t, size_t> indexToPos;

    inline void recomputePositions() noexcept;
    inline BibEntry* getEntryFromIndex(uint64_t index);

    inline std::string getPrepArXivFromEntry(const BibEntry* entry) noexcept{
        return this->getArXivId(entry);
    }

    inline std::string getPrepDOIFromEntry(const BibEntry* entry) noexcept{
        if(!(this->hasDOI(entry))){return "";};
        const std::string doi = entry->attributes.at("doi");
        return this->prepDOI(doi);
    }

    Parser directParser;
};