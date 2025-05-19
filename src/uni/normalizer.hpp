#pragma once

#include <string>

#include <unicode/unistr.h>
#include <unicode/errorcode.h>
#include <unicode/normalizer2.h>

#include "../utils/attributes.hpp"

class StringNormalizer {
public:
    StringNormalizer(UErrorCode &errorCode)
        :   nfkc(*icu::Normalizer2::getNFKCInstance(errorCode)),
            nfc(*icu::Normalizer2::getNFCInstance(errorCode)),
            nfd(*icu::Normalizer2::getNFDInstance(errorCode)),
            fcd(*icu::Normalizer2::getInstance(NULL, "nfc", UNORM2_FCD, errorCode)) {}

    virtual ~StringNormalizer(){}
    
    bool toNFKC(const icu::UnicodeString &s, UErrorCode &errorCode, icu::UnicodeString &NFKCString) const noexcept;
    bool toNFC(const icu::UnicodeString &s, UErrorCode &errorCode, icu::UnicodeString &NFCString) const noexcept;
    bool toNFD(const icu::UnicodeString &s, UErrorCode &errorCode, icu::UnicodeString &NFCString) const noexcept;
    bool toFCD(const icu::UnicodeString &s, UErrorCode &errorCode, icu::UnicodeString &fcdString) const noexcept;

private:
    const icu::Normalizer2 &nfkc;
    const icu::Normalizer2 &nfc;
    const icu::Normalizer2 &nfd;
    const icu::Normalizer2 &fcd;
};