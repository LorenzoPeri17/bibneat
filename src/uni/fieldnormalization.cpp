#include "fieldnormalization.hpp"

void FieldNormalizer::addUTF8Preamble() noexcept{
    const std::string body = "\""
    "\\makeatletter"
    "\\@ifpackageloaded{inputenc}{\\inputencoding{utf8}}{}"
    "\\@ifpackageloaded{fontenc}{\\fontencoding{T1}\\selectfont}{}"
    "\\makeatother"
    "\"";
    std::unique_ptr<SpecialEntry> entry = std::make_unique<SpecialEntry>("preamble", body);
    this->bibDB->addSpecialEntry(std::move(entry));
}

void FieldNormalizer::NFCNormalize() noexcept{

    icu::UnicodeString srcUString;
    icu::UnicodeString fcdUString;
    icu::UnicodeString resUString;
    bool success;
    for (auto& entry: this->bibDB->entries){

        if (!(entry->keep)){
            continue;
        }

        for(auto& [key,field] : entry->keepAttributes){

            if(!(entry->keepAttributes.at(key))){
                continue; // no use normalizing the field
            }

            srcUString = toUS(entry->attributes[key]);
            success = this->normalizer.toFCD(srcUString, this->UError, fcdUString);
            if(!success){
                continue; // something went wrong...
            }
            success = this->normalizer.toNFC(fcdUString, this->UError, resUString);
            if(!success){
                continue; // something went wrong...
            }
            std::string res="";
            resUString.toUTF8String(res);
            entry->attributes[key] = res;
        }
    
    }
}

void FieldNormalizer::NFKCNormalize() noexcept{

    icu::UnicodeString srcUString;
    icu::UnicodeString fcdUString;
    icu::UnicodeString resUString;
    bool success;
    for (auto& entry: this->bibDB->entries){

        if (!(entry->keep)){
            continue;
        }

        for(auto& [key,field] : entry->keepAttributes){

            if(!(entry->keepAttributes.at(key))){
                continue; // no use normalizing the field
            }

            srcUString = toUS(entry->attributes[key]);
            success = this->normalizer.toFCD(srcUString, this->UError, fcdUString);
            if(!success){
                continue; // something went wrong...
            }
            success = this->normalizer.toNFKC(fcdUString, this->UError, resUString);
            if(!success){
                continue; // something went wrong...
            }
            std::string res="";
            resUString.toUTF8String(res);
            entry->attributes[key] = res;
        }
    
    }
}

void FieldNormalizer::uni2latex() noexcept{

    bool success;
    for (auto& entry: this->bibDB->entries){

        if (!(entry->keep)){
            continue;
        }

        for(auto& [key,field] : entry->keepAttributes){

            if(!(entry->keepAttributes.at(key))){
                continue;
            }

            std::string res="";
            success = this->uni2latexString(entry->attributes.at(key), res);
            if(!success){continue;}
            entry->attributes[key] = res;
        }
    }

}

bool FieldNormalizer::uni2latexString(const std::string &srcStr, std::string &newStr) noexcept{

    icu::UnicodeString srcUString = toUS(srcStr);
    icu::UnicodeString tmpUString, resUString;
    bool success;

    // canonical decomposition
    success = this->normalizer.toFCD(srcUString, this->UError, resUString);
    if(!success)[[unlikely]]{return false;}
    success = this->normalizer.toNFD(resUString, this->UError, tmpUString);
    if(!success)[[unlikely]]{return false;}
    // NFD-decomposed string is now in tmpUString

    // handle diacritics
    for (auto& [uni, tex]: this->utf82latexEnc.texDiacritics){
        icu::RegexMatcher matcher (USTRLITERAL("(\\p{Letter}[" DIACRITICSGROUP "]*?)") + uni, UREGEX_CASE_INSENSITIVE, this->UError);
        icu::UnicodeString repl = tex + USTRLITERAL("{$1}");
        matcher.reset(tmpUString);
        tmpUString = matcher.replaceAll(repl, this->UError);
        if(U_FAILURE(this->UError))[[unlikely]]{return false;}
    }

    // special characters
    for (auto& [uni, tex]: this->utf82latexEnc.texSpecialChars){
        tmpUString.findAndReplace(uni, tex);
    }

    // re-normalize
    success = this->normalizer.toNFC(tmpUString, this->UError, resUString);
    if(!success)[[unlikely]]{return false;}

    resUString.toUTF8String(newStr);
    return true;
}