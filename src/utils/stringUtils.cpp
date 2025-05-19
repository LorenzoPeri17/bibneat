#include "stringUtils.hpp"

void toLower(std::string *str){
    std::transform(str->begin(), str->end(), str->begin(), [](unsigned char c){ return std::tolower(c); });
}

void eraseWhiteSpace(std::string *str){
    std::erase_if(*str, isspace);
}

CONSTANT static bool isTabLFCR(const int c){
    return (c=='\t')|(c=='\n')|(c=='\r');
}

void eraseTabLFCR(std::string *str){
    // So, we actually want to put a space there just in case...
    for(size_t i = 0; i < str->length(); i++) {
        if(isTabLFCR(str->at(i))) {
            str->at(i)=' ';
        }

    }
}

void eraseDoubleSpaces(std::string *str){
    for(size_t i = 0; i < str->length() - 1; i++) {
        if(str->at(i) == ' ' && str->at(i + 1) == ' ') {
            str->erase(str->begin()+i);
            i--;
        }

    }
}

bool string_contains(const std::string &strFrom, const std::string &subStr){
    std::size_t found = strFrom.find(subStr);
    if (found != std::string::npos){
        return true;
    } else{
        return false;
    }
}

bool stringStartsWith(const std::string &str, const std::string &prefix){
    return (str.compare(0, prefix.length(), prefix) == 0);
}

void replaceOnce(std::string& str, std::string const& toReplace, std::string const& replaceWith) {
    std::size_t pos = str.find(toReplace);
    if (pos == std::string::npos) return;
    str.replace(pos, toReplace.length(), replaceWith);
}
