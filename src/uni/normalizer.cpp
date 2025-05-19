#include "normalizer.hpp"

bool StringNormalizer::toNFKC(const icu::UnicodeString &s, UErrorCode &errorCode, icu::UnicodeString &NFKCString) const noexcept{
        NFKCString = this->nfkc.normalize(s, errorCode);
        return U_SUCCESS(errorCode);
    }

bool StringNormalizer::toNFC(const icu::UnicodeString &s, UErrorCode &errorCode, icu::UnicodeString &NFCString) const noexcept{
    NFCString = this->nfc.normalize(s, errorCode);
    return U_SUCCESS(errorCode);
}

bool StringNormalizer::toNFD(const icu::UnicodeString &s, UErrorCode &errorCode, icu::UnicodeString &NFDString) const noexcept{
    NFDString = this->nfd.normalize(s, errorCode);
    return U_SUCCESS(errorCode);
}

bool StringNormalizer::toFCD(const icu::UnicodeString &s, UErrorCode &errorCode, icu::UnicodeString &fcdString) const noexcept{
    int32_t spanQCYes=this->fcd.spanQuickCheckYes(s, errorCode);
    if(U_FAILURE(errorCode))[[unlikely]]{
        return false;
    }
    if(spanQCYes==s.length())[[likely]]{
        fcdString=s;
        return true;  // s is already in FCD
    } else {
        icu::UnicodeString unnormalized=s.tempSubString(spanQCYes);
        fcdString.setTo(false, s.getBuffer(), spanQCYes);
        this->fcd.normalizeSecondAndAppend(fcdString, unnormalized, errorCode);
        return U_SUCCESS(errorCode);
    }
}