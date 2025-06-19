#include "printer.hpp"

bool Printer::toBibFile(std::string fname, bool overwrite){

    this->fname = fname;

    std::ios_base::openmode openMode;

    if (overwrite){
        openMode = std::ios::trunc;
    } else {
        openMode = std::ios::app;
    }
    replaceTilde(fname);
    this->file = std::ofstream(fname, openMode);
    if (!this->file.is_open()) {
        return false;
    }
    this->stream=dynamic_cast<std::ostream*>(&file);

    this->writeDBToStream();

    this->file.close();
    this->stream = nullptr;
    return true;
}

std::string Printer::toString(){
    std::ostringstream oss;
    this->stream = dynamic_cast<std::ostream*>(&oss);

    this->writeDBToStream();

    this->stream = nullptr;
    return oss.str();
}

void Printer::writeDBToStream() noexcept{

    for (auto& specialEntry: this->bibDB->specialEntries){
        // This is fine because we guarantee that entry will survive the call
        // we do not want to use std::move because we want bibDB to retain ownership
        this->writeSpecialEntry(specialEntry.get()); 
    }

    for (auto& entry: this->bibDB->entries){
        if(!(entry->keep)){
            continue;
        }
        // This is fine because we guarantee that entry will survive the call
        // we do not want to use std::move because we want bibDB to retain ownership
        this->writeBibEntry(entry.get()); 
    }

}

void Printer::writeBibEntry(BibEntry *entry) noexcept{

    *(this->stream) << "@" << entry->bibTypeName << "{" << entry->bibKey << "," << std::endl;

    std::unordered_map<std::string,std::string>::const_iterator found;
    for (auto& key : this->priorityKeys){
        found = entry->attributes.find(key);
        if (found != entry->attributes.end()){ // found
            if (entry->keepAttributes.find(key)->second){
                *(this->stream) << "  " << key << "={" << found->second << "}," << std::endl;
            }
        }
    }

    bool skipKey;
    bool writeComma = false;
    for(auto& [key, value] : entry->attributes){
        skipKey = false;
        for (auto& usedKey : this->priorityKeys){
            if (key==usedKey) {
                skipKey = true;
                break;
            } // avoid duplicates
        }
        if (skipKey) continue;
        if (entry->keepAttributes.find(key)->second){
            if(writeComma){
                *(this->stream) << "," << std::endl;
            }
            *(this->stream) << "  " << key << "={" << value << "}";
            // trailing commas are NOT allowed in BibTex...
            writeComma = true;
        }
    }

    *(this->stream) << std::endl << "}" << std::endl << std::endl;
}

void Printer::writeSpecialEntry(SpecialEntry *entry) noexcept{
    *(this->stream) << "@" << entry->bibTypeName << "{" << entry->body << "}" << std::endl << std::endl;
}