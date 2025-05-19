#pragma once

#include <string>

#include "../utils/debug.hpp"
#include "../utils/attributes.hpp"

#include "../database/bibfile.hpp"
#include "../database/parser.hpp"

#include <unicode/regex.h>

#include "normalizer.hpp"
#include "icuUtils.hpp"
#include "utf82latex.hpp"

class FieldNormalizer{
public:

    explicit FieldNormalizer(
        std::shared_ptr<BibDB> bibDB
        ):  UError(),
            bibDB(bibDB),
            normalizer(this->UError){
                this->UError = U_ZERO_ERROR;
            }

    virtual ~FieldNormalizer(){}

    UErrorCode UError;

    void addUTF8Preamble() noexcept;

    void NFCNormalize() noexcept;
    void NFKCNormalize() noexcept;

    void uni2latex() noexcept;

private:
    std::shared_ptr<BibDB> bibDB;
    StringNormalizer normalizer;
    const UTF82LatexEnc utf82latexEnc = UTF82LatexEnc();

    bool uni2latexString(const std::string &srcStr, std::string &newStr) noexcept;

};