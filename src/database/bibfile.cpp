#include "./bibfile.hpp"

uint64_t BibEntry::indexGen = 0;

BibEntry::BibEntry(
    std::string bibTypeName, 
    std::string bibKey) : 
    bibKey(bibKey), 
    bibTypeName(bibTypeName),
    index(this->indexGen++)
    {
    this->keep = true;
    this->doiChecked = false;
    this->arxivChecked = false;
    this->bibType = BT_NONE;
    this->attributes = std::unordered_map<std::string, std::string>();
    this->attributes.reserve(ENTRY_ATTRIBUTE_RESERVE);
    this->keepAttributes = std::unordered_map<std::string, bool>();
    this->keepAttributes.reserve(ENTRY_ATTRIBUTE_RESERVE);
}

INLINE static bool checkArXivInField(const BibEntry* entry, const std::string &key){
    if (KEY_EXISTS(entry->attributes,key)){
        const std::string attribute = entry->attributes.at(key);
        // careful! this check is is case sensitive
        if(string_contains(attribute, "arxiv")){
            return true;
        } else if(string_contains(attribute, "arXiv")){
            return true;
        }
    }
    return false;
}

void BibEntry::checkArXiv() noexcept{

    ASSERTMSG(((this->bibTypeName=="misc")||(this->bibTypeName=="article")), "Only misc and article types should be checked for arxiv");

    if ((this->bibTypeName=="article")&&(!checkArXivInField(this, "journal"))){
        this->bibType =  BT_ARTICLE;
    }

    auto keysToCheck = {"url", "archiveprefix", "doi", "note", "number"};

    for (auto& key : keysToCheck){
        if(checkArXivInField(this, key)){
            this->bibTypeName="misc"; // you'd be surprised how many times reference managers default to article...
            this->bibType =  BT_ARXIV;
        }
    }
}

void BibEntry::findBibType() noexcept{
    const BibType bibType=getBibType(this->bibTypeName);
    this->bibType = bibType;
    if ((bibType==BT_ARTICLE)||(bibType==BT_MISC)){
        this->checkArXiv();
    }
}

void BibEntry::addAttribute(std::string tag, std::string body){
    if(body.empty()){return;}
    toLower(&tag);
    eraseWhiteSpace(&tag);
    eraseTabLFCR(&body);
    eraseDoubleSpaces(&body);
    this->attributes.insert_or_assign(tag, body);
    this->keepAttributes.insert_or_assign(tag, true);
}

void BibEntry::cleanup(){
    this->attributes.clear();
    this->keepAttributes.clear();
}

BibDB::BibDB(){
    this->numEntries = 0;
    this->numSpecialEntries = 0;
    this->entries = std::vector<std::unique_ptr<BibEntry>> ();
    this->specialEntries = std::vector<std::unique_ptr<SpecialEntry>> ();
    this->entries.reserve(DB_ENTRIES_RESERVE);
}

void BibDB::addEntry(std::unique_ptr<BibEntry> newEntry) {
    newEntry->findBibType();
    this->entries.emplace_back(std::move(newEntry));
    this->numEntries++;
    ASSERTMSG((this->entries.back()!=nullptr),"Added NULL to the BibDB...");
}

void BibDB::addSpecialEntry(std::unique_ptr<SpecialEntry> newEntry) {
    this->specialEntries.emplace_back(std::move(newEntry));
    this->numSpecialEntries++;
}

void BibDB::clear(){
    this->entries.clear();
    this->numEntries=0;
}

void BibDB::cleanup(){
    for(size_t i = 0; i < this->entries.size(); i++) {
        if(!(this->entries.at(i)->keep)){
            this->entries.erase(this->entries.begin()+i);
            i--;
            this->numEntries--;
        }
    }
}

uint64_t BibDB::countKeep() const{
    uint64_t keep = 0;
    for(auto& entry: this->entries){
        if(entry->keep){
            keep++;
        }
    }
    return keep;
}