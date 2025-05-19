#pragma once

#include <array>

#include <unicode/unistr.h>

#define USTRLITERAL(s)\
    icu::UnicodeString::fromUTF8(icu::StringPiece(s))

#define DIACRITICSGROUP "\\u0300-\\u0362\\u20D0-\\u20E3"

#define BKSLASH "\\u005C"

class UTF82LatexEnc{
public:
    explicit UTF82LatexEnc(){}

    virtual ~UTF82LatexEnc(){}

const std::array<std::pair<const icu::UnicodeString,const icu::UnicodeString>,14> texDiacritics = {
    std::make_pair(USTRLITERAL("\u0300"), USTRLITERAL(BKSLASH "`")),
    std::make_pair(USTRLITERAL("\u0301"), USTRLITERAL(BKSLASH "'")),
    std::make_pair(USTRLITERAL("\u0302"), USTRLITERAL(BKSLASH "^")),
    std::make_pair(USTRLITERAL("\u0303"), USTRLITERAL(BKSLASH "~")),
    std::make_pair(USTRLITERAL("\u0304"), USTRLITERAL(BKSLASH "=")),
    // \u0305 not supported afaik
    std::make_pair(USTRLITERAL("\u0306"), USTRLITERAL(BKSLASH "u")),
    std::make_pair(USTRLITERAL("\u0307"), USTRLITERAL(BKSLASH ".")),
    std::make_pair(USTRLITERAL("\u0308"), USTRLITERAL(BKSLASH "\u0022")),
    // 
    std::make_pair(USTRLITERAL("\u030A"), USTRLITERAL(BKSLASH "r")),
    std::make_pair(USTRLITERAL("\u030B"), USTRLITERAL(BKSLASH "H")),
    std::make_pair(USTRLITERAL("\u030C"), USTRLITERAL(BKSLASH "v")),
    //
    std::make_pair(USTRLITERAL("\u0323"), USTRLITERAL(BKSLASH "d")),
    //
    std::make_pair(USTRLITERAL("\u0327"), USTRLITERAL(BKSLASH "c")),
    std::make_pair(USTRLITERAL("\u0328"), USTRLITERAL(BKSLASH "k")),
};

const std::array<std::pair<const icu::UnicodeString,const icu::UnicodeString>,15> texSpecialChars = {
    std::make_pair(USTRLITERAL("\u00e6"), USTRLITERAL("{\\ae}")),
    std::make_pair(USTRLITERAL("\u00c6"), USTRLITERAL("{\\AE}")),
    std::make_pair(USTRLITERAL("\u0157"), USTRLITERAL("{\\oe}")),
    std::make_pair(USTRLITERAL("\u0152"), USTRLITERAL("{\\OE}")),
    std::make_pair(USTRLITERAL("\u00f8"), USTRLITERAL("{\\o}")),
    std::make_pair(USTRLITERAL("\u00d8"), USTRLITERAL("{\\O}")),
    std::make_pair(USTRLITERAL("\u0141"), USTRLITERAL("{\\L}")),
    std::make_pair(USTRLITERAL("\u0142"), USTRLITERAL("{\\l}")),
    std::make_pair(USTRLITERAL("\u0142"), USTRLITERAL("{\\l}")),
    std::make_pair(USTRLITERAL("\u0131"), USTRLITERAL("{\\i}")),
    std::make_pair(USTRLITERAL("\u0237"), USTRLITERAL("{\\j}")),
    std::make_pair(USTRLITERAL("\u00a1"), USTRLITERAL("{!`}")),
    std::make_pair(USTRLITERAL("\u00bf"), USTRLITERAL("{?`}")),
    std::make_pair(USTRLITERAL("\u2013"), USTRLITERAL("-")),
    std::make_pair(USTRLITERAL("\u2009"), USTRLITERAL(" ")),
};

};