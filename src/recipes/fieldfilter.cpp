#include "fieldfilter.hpp"

void FieldFilter::keepBibTex() noexcept{

    BibType bibType;

    for (auto& entry: this->bibDB->entries){

        bibType = entry->bibType;
        ASSERTMSG(bibType!=BT_NONE, "BT_NONE should never leave the parser stage...\n");

        const std::unordered_set<std::string> keep = this->BTFields.keepBibTexFields.at(bibType);

        for(auto& [key, _] : entry->keepAttributes){
            if((this->BTFields.keepAlways.contains(key))||(keep.contains(key))){
                entry->keepAttributes[key] = true;
            } else {
                entry->keepAttributes[key] = false;
            }
        }

    }
}
