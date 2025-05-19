#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <memory>

#include "../utils/debug.hpp"
#include "../utils/attributes.hpp"
#include "../utils/stringUtils.hpp"
#include "../utils/pathUtils.hpp"

#include "bibfile.hpp"
#include "tokens.hpp"

#include <iostream>

#define STRING_RESERVE_SIZE 4096

class Parser {
public:

    explicit Parser(
        std::shared_ptr<BibDB> bibDB,
        const bool verbose = false
        ): bibDB(bibDB),
            verbose(verbose){
            this->currentStr.reserve(STRING_RESERVE_SIZE);
        }

    virtual ~Parser() {
        if (this->file.is_open()) {
            this->file.close();
        }
    }

    bool parse(const std::string fname);
    bool parseString(const std::string bibString);
    std::string currentStr;

private:
    // bib file
    std::string fname;
    std::istream *stream;
    std::ifstream file;
    std::shared_ptr<BibDB> bibDB;
    void reset() noexcept;

    // verbose
    const bool verbose;

    // creating the database
    bool handleNewEntry();
    void handleSpecialEntry(const std::string bibTypeName);

    // state
    bool isInEntry;
    bool isInAttribute;
    bool attrOpenWithQuotes;
    bool isInMath;
    bool isEscaped;
    bool immediate;
    bool needKey;
    bool needOneWordAttr;
    int curlyDepth;

    // lexing and parsing
    int LastChar;
    Token getNext() noexcept;
    void nextToken() noexcept;
    bool parseLoop();
    void skipWhiteSpace(bool) noexcept;
    void skipJunk(bool) noexcept;
    Token currentToken;
    const std::unordered_set<int> specialChars {'{','}','$','@','\\','"'};

    const std::unordered_set<std::string> specialBibTypes {"string", "preamble", "comment"};
};