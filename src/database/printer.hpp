#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <array>
#include <memory>

#include "../utils/debug.hpp"
#include "../utils/attributes.hpp"
#include "../utils/pathUtils.hpp"

#include "bibfile.hpp"

class Printer {
public:

    explicit Printer(
        std::shared_ptr<BibDB> bibDB
        ): bibDB(bibDB){}

    virtual ~Printer(){}

    bool toBibFile(std::string fname, bool overwrite = true);

private:
    // bib file
    std::string fname;
    std::ofstream file;
    std::shared_ptr<BibDB> bibDB;
    void writeBibEntry(BibEntry *entry) noexcept;
    void writeSpecialEntry(SpecialEntry *entry) noexcept;

    // styling
    const std::array<std::string,4> priorityKeys = {"title","author","year","journal"};
};