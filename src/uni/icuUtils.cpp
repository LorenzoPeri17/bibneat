#include "icuUtils.hpp"

icu::UnicodeString toUS(const std::string s){
    return icu::UnicodeString::fromUTF8(icu::StringPiece(s.c_str()));
}