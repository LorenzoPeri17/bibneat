#include "jsapihandler.hpp"

#define MIN(a,b) \
        (a < b) ? a : b

inline void JsApiHandler::recomputePositions() noexcept{
    size_t pos = 0;
    this->indexToPos.clear();
    for (auto& entry: this->bibDB->entries){
        if (entry==nullptr){
            pos++;
            continue;
        }
        this->indexToPos[entry->index]=pos++;
    }
}

std::map<uint64_t,std::string> JsApiHandler::getArXivIds(){

    std::map<uint64_t,std::string> resHolder;
    for (auto& entry: this->bibDB->entries){
        if (entry==nullptr){
            continue;
        } 
        if(!(entry->keep)){
            continue;
        }
        if (entry->arxivChecked){
            continue;
        }
        if(entry->bibType == BT_ARXIV){
            resHolder[entry->index] = this->getPrepArXivFromEntry(entry.get());
        }
    }
    return resHolder;
}

std::map<uint64_t,std::string> JsApiHandler::getDOIs(){

    std::map<uint64_t,std::string> resHolder;
    for (auto& entry: this->bibDB->entries){
        if (entry==nullptr){
            continue;
        } 
        if(!(entry->keep)){
            continue;
        }
        if (entry->doiChecked){
            continue;
        }
        BibEntry *pEntry = entry.get();
        if(this->hasDOI(pEntry)){
            resHolder[entry->index] = this->getPrepDOIFromEntry(pEntry);
            entry->doiChecked = true;
        }
    }
    return resHolder;
}

inline BibEntry* JsApiHandler::getEntryFromIndex(uint64_t index){
    if(!(KEY_EXISTS(this->indexToPos, index)))[[unlikely]]{this->recomputePositions();}
    if(!(KEY_EXISTS(this->indexToPos, index)))[[unlikely]]{return nullptr;}
    size_t pos=this->indexToPos.at(index);
    if(pos>this->bibDB->numEntries-1)[[unlikely]]{return nullptr;}
    BibEntry* entry=this->bibDB->entries.at(pos).get();
    if(entry->index!=index)[[unlikely]]{return nullptr;}
    return entry;
}

std::map<uint64_t,std::string> JsApiHandler::updateArXivFromResponseAndGetDOIs(
                        std::vector<uint64_t> indices, 
                        std::vector<std::string> bibTex,
                        const bool doReplace){
    const uint64_t nEntries = MIN(indices.size(), bibTex.size());
    size_t ncalls = 0;
    uint64_t indexToReturn;
    BibEntry *oldEntry;
    BibEntry *newEntry;
    if(doReplace){
        this->recomputePositions();
    }
    this->shadowDB->clear();
    std::map<uint64_t,std::string> resHolder;
    for(size_t idx = 0; idx<nEntries; idx++){
        auto entryIndex = indices.at(idx);
        auto tex = bibTex.at(idx);
        if(!(this->shadowParser.parseString(tex))){continue;};
        newEntry = this->shadowDB->entries.back().get();
        oldEntry = this->getEntryFromIndex(entryIndex);
        if(!oldEntry){continue;}
        if(doReplace){
            oldEntry->keep=false;
            newEntry->bibKey = oldEntry->bibKey;
            newEntry->arxivChecked = true;
            indexToReturn = newEntry->index;
            this->bibDB->addEntry(std::move(this->shadowDB->entries.back()));
            this->shadowDB->clear(); 
            if((++ncalls)>=this->N_REQUEST_BEFORE_CLEAN){
                this->bibDB->cleanup();
                this->recomputePositions();
                ncalls=0;
            }
        } else{
            indexToReturn = oldEntry->index;
        }
        if(this->hasDOI(newEntry)){
            resHolder[indexToReturn] = this->getPrepDOIFromEntry(newEntry);
        }
        this->shadowDB->clear(); 
    }
    this->bibDB->cleanup();
    return resHolder;
}

std::string JsApiHandler::getBibKeysFromIndex(uint64_t index){
    BibEntry* entry = this->getEntryFromIndex(index);
    if(!entry){return "";}
    return entry->bibKey;
}

void JsApiHandler::setUnkeepFromIndex(uint64_t index){
    BibEntry* entry = this->getEntryFromIndex(index);
    if(entry){entry->keep = false;}
}

void JsApiHandler::updateDOIFromResponse(std::vector<uint64_t> indices, std::vector<std::string> bibTex){
    const uint64_t nEntries = MIN(indices.size(), bibTex.size());
    size_t ncalls = 0;
    BibEntry *oldEntry;
    BibEntry *newEntry;
    this->recomputePositions();
    for(size_t idx = 0; idx<nEntries; idx++){
        auto entryIndex = indices.at(idx);
        auto tex = bibTex.at(idx);
        oldEntry = this->getEntryFromIndex(entryIndex);
        if(!oldEntry){continue;}
        if(!(this->directParser.parseString(tex))){continue;};
        newEntry = this->bibDB->entries.back().get();
        oldEntry->keep=false;
        newEntry->bibKey = oldEntry->bibKey;
        newEntry->doiChecked = true;
        if((++ncalls)>=this->N_REQUEST_BEFORE_CLEAN){
            this->bibDB->cleanup();
            this->recomputePositions();
            ncalls=0;
        }
    }
    this->bibDB->cleanup();
}

std::map<uint64_t,std::string> JsApiHandler::addFromArXivResponseAndGetDoi(std::string res){
    this->directParser.parseString(res);
    BibEntry* entry = this->bibDB->entries.back().get();
    entry->arxivChecked = true;
    std::map<uint64_t,std::string> responseHolder;
    if(this->hasDOI(entry)){
        const std::string doi =  entry->attributes.at("doi");
        const uint64_t index = entry->index;
        responseHolder[index] = doi;
    }
    return responseHolder;
}

void JsApiHandler::addFromDOIResponse(std::string res){
    this->directParser.parseString(res);
    this->bibDB->entries.back()->doiChecked = true;
}