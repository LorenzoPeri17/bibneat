#include "jsapihandler.hpp"

inline void JsApiHandler::recomputePositions() noexcept{
    size_t idx = 0;
    this->indexToPos.clear();
    for (auto& entry: this->bibDB->entries){
        idx++;
        if (entry==nullptr){
            continue;
        }
        this->indexToPos.at(entry->index)=idx;
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
    if(!(KEY_EXISTS(this->indexToPos, index)))[[unlikely]]{return nullptr;}
    size_t pos=this->indexToPos.at(index);
    if(pos>this->bibDB->numEntries-1)[[unlikely]]{return nullptr;}
    BibEntry* entry=this->bibDB->entries.at(pos).get();
    if(entry->index!=index)[[unlikely]]{return nullptr;}
    return entry;
}

std::map<uint64_t,std::string> JsApiHandler::updateArXivFromResponseAndGetDOIs(
                        std::vector<std::pair<uint64_t, std::string>> res, 
                        const bool doReplace){
    size_t ncalls = 0;
    uint64_t indexToReturn;
    BibEntry *oldEntry;
    BibEntry *newEntry;
    if(doReplace){
        this->recomputePositions();
    }
    this->shadowDB->clear();
    std::map<uint64_t,std::string> resHolder;
    for(auto& [index, bibTex] : res){
        if(!(this->shadowParser.parseString(bibTex))){continue;};
        newEntry = this->bibDB->entries.back().get();
        if(doReplace){
            oldEntry = this->getEntryFromIndex(index);
            if(!oldEntry){continue;}
            oldEntry->keep=false;
            newEntry->bibKey = oldEntry->bibKey;
            newEntry->arxivChecked = true;
            this->bibDB->addEntry(std::move(this->shadowDB->entries.back()));
            indexToReturn = newEntry->index;
            this->shadowDB->clear(); 
            if((++ncalls)>=this->N_REQUEST_BEFORE_CLEAN){
                this->bibDB->cleanup();
                this->recomputePositions();
                ncalls=0;
            }
        } else{
            indexToReturn = index;
        }
        if(this->hasDOI(newEntry)){
            resHolder[indexToReturn] = this->getPrepDOIFromEntry(newEntry);
        }
        this->shadowDB->clear(); 
    }
    return resHolder;
}

std::string JsApiHandler::getBibKeysFromIndex(uint64_t index){
    BibEntry* entry = this->getEntryFromIndex(index);
    if(!entry){return "";}
    return entry->bibKey;
}

void JsApiHandler::updateDOIFromResponse(std::vector<std::pair<uint64_t, std::string>> res){
    size_t ncalls = 0;
    BibEntry *oldEntry;
    BibEntry *newEntry;
    this->recomputePositions();
    for(auto& [index, bibTex] : res){
        oldEntry = this->getEntryFromIndex(index);
        if(!oldEntry){continue;}
        if(!(this->directParser.parseString(bibTex))){continue;};
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
}

