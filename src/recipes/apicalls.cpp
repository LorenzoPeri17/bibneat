#include "apicalls.hpp"

bool ApiCaller::hasDOI(const BibEntry* entry) const noexcept{
    return KEY_EXISTS(entry->attributes,"doi");
}

const std::string ApiCaller::getArXivId(const BibEntry* entry) noexcept{

    std::smatch regexMatch;

    if (KEY_EXISTS(entry->attributes,"archiveprefix")){
        const std::string attribute = entry->attributes.at("archiveprefix");
        // careful! this check is is case sensitive
        if(string_contains(attribute, "arxiv") || string_contains(attribute, "arXiv")){
            if (KEY_EXISTS(entry->attributes,"eprint")){
                return entry->attributes.at("eprint");
            }
        }
    }

    if (KEY_EXISTS(entry->attributes,"url")){
        const std::string attribute = entry->attributes.at("url");
        if(std::regex_search(attribute,regexMatch,this->arXivUrlPattern,std::regex_constants::match_not_null)){
            return regexMatch[1];
        }
    }

    if (KEY_EXISTS(entry->attributes,"doi")){
        const std::string attribute = entry->attributes.at("doi");
        if(std::regex_search(attribute,regexMatch,this->arXivDOIPattern,std::regex_constants::match_not_null)){
            return regexMatch[1];
        }
    }

    if (KEY_EXISTS(entry->attributes,"note")){
        const std::string attribute = entry->attributes.at("note");
        if(std::regex_search(attribute,regexMatch,this->arXivNotePattern,std::regex_constants::match_not_null)){
            return regexMatch[1];
        }
    }

    if (KEY_EXISTS(entry->attributes,"number")){
        const std::string attribute = entry->attributes.at("number");
        if(std::regex_search(attribute,regexMatch,this->arXivNotePattern,std::regex_constants::match_not_null)){
            return regexMatch[1];
        }
    }

    return "";
}

const std::string ApiCaller::prepDOI(const std::string doi) noexcept{
    std::smatch regexMatch;
    std::regex_search(doi,regexMatch,this->DOIPattern,std::regex_constants::match_not_null);
    return regexMatch[1];
}

bool ApiCaller::getFromArXivApi(const BibEntry* entry) noexcept{

    long responseCode;
    const std::string arXivId = this->getArXivId(entry);
    if(arXivId.empty()){return false;}
    const RequestsCode rqc = getBibTexarXiv(arXivId.c_str(), arXivId.size(), this->CapiBib.get(), &(this->NapiBib), this->N_RESERVE_API_BIB, this->connTimeout, this->totTimeout, &responseCode);
    this->apiBib = std::string(this->CapiBib.get(), this->NapiBib-1);

    if(rqc == RQC_FOUND){
        return true;
    } else if (rqc == RQC_NOT_FOUND){
        std::cerr << "Entry " + entry->bibKey + " with arXiv Id " + arXivId + " not found on the arXiv API" << std::endl;
        return false;
    }else if (rqc == RQC_BAD_ID){
        std::cerr << "Malformed arXiv Id" + arXivId + "for entry " + entry->bibKey << std::endl;
        return false;
    } else{
        std::cerr << "Error contacting the arXiv API for entry " + entry->bibKey + " with arXiv Id " + arXivId << std::endl;
        return false;
    }
}

void ApiCaller::checkArXivEntry(BibEntry* entry, const bool replaceWithArxiv, const bool replaceWithDoi) noexcept{
    entry->arxivChecked = true;
    if(this->verbose){
        std::cout << "Looking for entry " << entry->bibKey << " on arxiv.org API"<<std::endl;
    }

    if(!this->getFromArXivApi(entry)){
        if(this->verbose){
            std::cout << "Entry " << entry->bibKey << " NOT found on arxiv.org API"<<std::endl;
        }
        return;
    }
    bool foundDoi;
    std::string doi;

    if(this->verbose){
        std::cout << "Parsing response for entry " << entry->bibKey << " from arxiv.org API"<<std::endl;
    }

    bool parseSuccess = this->shadowParser.parseString(this->apiBib);
    if(!parseSuccess)[[unlikely]]{return;} //maybe write a message to cerr?
    ASSERTMSG((this->shadowDB->numEntries==1),"Parsing succeeded but no entry in the shadow DB after arXiv...");
    BibEntry* apiEntry = this->shadowDB->entries[0].get();
    apiEntry->bibKey=entry->bibKey;
    apiEntry->bibType = BT_ARXIV;

    if(this->verbose){
        std::cout << "Entry " << entry->bibKey << " found on arxiv.org API"<<std::endl;
    }

    if(this->hasDOI(apiEntry)){
        foundDoi = true;
        doi = apiEntry->attributes.at("doi");
    } else{
        foundDoi = false;
    }

    if (replaceWithArxiv){
        entry->keep=false;
        this->bibDB->addEntry(std::move(this->shadowDB->entries[0]));
        entry=this->bibDB->entries.back().get();
    }

    this->shadowDB->clear(); // we can delete apiEntry
    apiEntry = nullptr;
    
    if(foundDoi){
        if(this->verbose){
                std::cout << "Found published DOI for entry " << entry->bibKey <<std::endl;
        }
        if(replaceWithDoi){
            this->checkDOIEntry(entry, replaceWithDoi, doi, true);
        } else{
            std::cout << std::endl << "ArXiv entry " << entry->bibKey << " has been published with DOI"<<std::endl;
            std::cout << doi << std::endl;
            std::cout << "Please consider replacing with published version"<< std::endl << std::endl;
        }
    }

}

void ApiCaller::checkArXiv(const bool replaceWithArxiv, const bool replaceWithDoi) noexcept{
    size_t ncalls;
    bool didBreak;
    /*
    A little tale of this hack:
    For very large bib file(s), we already have a _huge_ vector holding on to references 
    to large chunks of memory because of all the strings for each entries.
    Some reference managers (ehm Zotero, Mendeley, ...) sometimes put THE ENTIRE ABSTRACT in the bib file. 
    One may get to the point of having 100kb-1Mb of text.
    When we call checkArXivEntry we can (worst case scenario) DOUBLE the number of entries, 
    half of which are marked (keep==false), so holding on to effectively dead memory.
    This is even worse in the case of DOI checks (virtually every entry has a DOI). 
    So if we stop for a sec each number of calls to erase entries we were not gonna keep anyway, 
    we make the memory footprint of bibneat significantly smaller.
    */
arxivloop:
    ncalls = 0;
    didBreak=false;
    for (auto& entry: this->bibDB->entries){
        if (entry==nullptr){
            continue;
        }
        if (!(entry->keep)){
            continue;
        }
        if (entry->arxivChecked){
            continue;
        }
        if (entry->bibType == BT_ARXIV){
            this->checkArXivEntry(entry.get(), replaceWithArxiv, replaceWithDoi);
            ncalls++;
        }
        if(ncalls>=this->N_REQUEST_BEFORE_CLEAN){
            didBreak=true;
            break;
        }
    }
    this->bibDB->cleanup();
    if(didBreak){
        goto arxivloop;
    }
}


bool ApiCaller::getFromDOIApi(const BibEntry* entry) noexcept{
    if (!(this->hasDOI(entry))){
        return false;
    }
    const std::string doi = entry->attributes.at("doi");
    if(string_contains(doi,"arXiv")){return this->getFromArXivApi(entry);}
    const RequestsCode rqc = this->RCgetFromDOIApi(doi);

    if(rqc == RQC_FOUND){
        return true;
    } else if (rqc == RQC_NOT_FOUND){
        std::cerr << "Entry " + entry->bibKey + " with doi " + doi + " not found on the DOI API" << std::endl;
        return false;
    } else if (rqc == RQC_BAD_ID){
        std::cerr << "Malformed DOI" + doi + "for entry " + entry->bibKey << std::endl;
        return false;
    } else{
        std::cerr << "Error contacting the DOI API for entry " + entry->bibKey << std::endl;
        return false;
    }
}

bool ApiCaller::getFromDOIApi(const std::string doi) noexcept{

    const RequestsCode rqc = this->RCgetFromDOIApi(doi);

    if(rqc == RQC_FOUND){
        return true;
    } else {
        return false;
    }
}

RequestsCode ApiCaller::RCgetFromDOIApi(const std::string doi) noexcept{
    long responseCode;
    const std::string doiSanitized = this->prepDOI(doi);
    if(doiSanitized.empty()){return RQC_BAD_ID;}
    if(string_contains(doi,"arXiv")){return RQC_BAD_ID;}
    const RequestsCode rqc = getBibTexDOI(doiSanitized.c_str(), doiSanitized.size(), this->CapiBib.get(), &(this->NapiBib), this->N_RESERVE_API_BIB, this->connTimeout, this->totTimeout, &responseCode);
    ASSERTMSG((this->NapiBib<this->N_RESERVE_API_BIB), "The DOI request overran the allocated buffer...");
    this->apiBib = std::string(this->CapiBib.get(), this->NapiBib-1);
    return rqc;
}

void ApiCaller::checkDOIEntry(BibEntry* entry, const bool replaceWithDoi,const std::string doi="", const bool useString=false) noexcept{

    bool apiFound;
    entry->doiChecked = true;

    if(this->verbose){
        std::cout << "Looking for entry " << entry->bibKey << " on DOI API"<<std::endl;
    }

    if (useString){
        apiFound = this->getFromDOIApi(doi);
    } else {
        apiFound = this->getFromDOIApi(entry);
    }

    if(!apiFound){
        return;
    }
    if(stringStartsWith(this->apiBib,"<!DOCTYPE html>")){
        return;
    }

    if(this->verbose){
        std::cout << "Parsing response for entry " << entry->bibKey << " from DOI API"<<std::endl;
    }

    bool parseSuccess = this->shadowParser.parseString(this->apiBib);
    if(!parseSuccess)[[unlikely]]{
        if(this->verbose){
            std::cerr << "Malformed response for entry " << entry->bibKey << " from DOI API"<<std::endl;
        }
        return;
    } //maybe write a message to cerr?
    ASSERTMSG((this->shadowDB->numEntries==1),"Parsing succeeded but no entry in the shadow DB after DOI...");
    BibEntry* apiEntry = this->shadowDB->entries[0].get();
    apiEntry->bibKey=entry->bibKey;
    apiEntry->doiChecked = true;

    if(!apiFound){
        if(this->verbose){
            std::cout << "Entry " << entry->bibKey << " NOT found on DOI API"<<std::endl;
        }
    } else {
        if(this->verbose && replaceWithDoi){
            std::cout << "Entry " << entry->bibKey << " found on DOI API"<<std::endl;
        }
    }

    if (replaceWithDoi){
        entry->keep=false;
        entry->cleanup();
        this->bibDB->addEntry(std::move(this->shadowDB->entries.front()));
        this->shadowDB->clear();
    } else{
        std::cout << "Entry " << entry->bibKey << " found on DOI API"<<std::endl;
    }

    this->shadowDB->clear(); 
}

void ApiCaller::checkDOI(const bool replaceWithDoi) noexcept{
    size_t ncalls;
    bool didBreak;
doiloop:
    ncalls = 0;
    didBreak=false;
    for (auto& entry: this->bibDB->entries){
        if (entry==nullptr){
            continue;
        }
        if (!(entry->keep)){
            continue;
        }
        if (entry->doiChecked){
            continue;
        }
        BibEntry *pEntry = entry.get();
        if (this->hasDOI(pEntry)){
            this->checkDOIEntry(pEntry, replaceWithDoi);
            ncalls++;
        }
        if(ncalls>=this->N_REQUEST_BEFORE_CLEAN){
            didBreak=true;
            break;
        }
    }
    this->bibDB->cleanup();
    if(didBreak){
        goto doiloop;
    }
}

const std::string ApiCaller::prepArXiv(const std::string arxiv) noexcept{
    std::smatch regexMatch;
    std::regex_search(arxiv,regexMatch,this->arXivImmediatePattern,std::regex_constants::match_not_null);
    return regexMatch[1];
}

bool ApiCaller::getArXivImmediate(const std::string arxiv, const bool replaceWithDoi) noexcept{

    bool foundDoi;
    std::string doi;
    long responseCode;
    const std::string arXivId = this->prepArXiv(arxiv);
    if(arXivId.empty()){
        std::cerr<< "Invalid arXiv entry for " + arxiv <<std::endl;
        return false;
    }

    const RequestsCode rqc = getBibTexarXiv(arXivId.c_str(), arXivId.size(), this->CapiBib.get(), &(this->NapiBib), this->N_RESERVE_API_BIB, this->connTimeout, this->totTimeout, &responseCode);
    this->apiBib = std::string(this->CapiBib.get(), this->NapiBib-1);

    if(rqc!=RQC_FOUND){
        std::cerr<< "Error getting " + arxiv + " from arXiv.org API"<<std::endl;
        return false;
    }
    bool parseSuccess = this->shadowParser.parseString(this->apiBib);
    if(!parseSuccess){
        std::cerr<< "Malformed response for " + arxiv + " from arxiv.org API"<<std::endl;
        std::cerr<< this->shadowParser.currentStr<<std::endl;
        return false;
    }
    BibEntry* apiEntry = this->shadowDB->entries[0].get();
    apiEntry->bibType = BT_ARXIV;

    if(this->hasDOI(apiEntry)){
        foundDoi = true;
        doi = apiEntry->attributes.at("doi");
    } else{
        foundDoi = false;
    }

    if(foundDoi){
        if(replaceWithDoi){
            this->shadowDB->clear(); 
            return this->getDOIImmediate(doi);
        } else{
            this->bibDB->addEntry(std::move(this->shadowDB->entries[0]));
            this->shadowDB->clear(); 
            std::cout << std::endl << "ArXiv entry " << arxiv << " has been published with DOI"<<std::endl;
            std::cout << doi << std::endl;
            std::cout << "Please consider replacing with published version"<< std::endl << std::endl;
        }
    }
    return true;
}

bool ApiCaller::getDOIImmediate(const std::string doi) noexcept{

    bool apiFound = this->getFromDOIApi(doi);
    if(!apiFound){
        std::cerr<< "Error getting " + doi + " from doi.org API"<<std::endl;
        return false;
    }
    bool parseSuccess = this->shadowParser.parseString(this->apiBib);
    if(!parseSuccess){
        std::cerr<< "Malformed response for " + doi + " from doi.org API"<<std::endl;
        std::cerr<< this->shadowParser.currentStr<<std::endl;
        return false;
    }
    BibEntry* apiEntry = this->shadowDB->entries[0].get();
    apiEntry->doiChecked = true;
    this->bibDB->addEntry(std::move(this->shadowDB->entries[0]));
    this->shadowDB->clear(); 
    return true;
}