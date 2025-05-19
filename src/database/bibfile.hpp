#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <locale>
#include <vector>
#include <memory>

#include <stdint.h>

#include "../utils/umap.hpp"
#include "../utils/stringUtils.hpp"
#include "../utils/debug.hpp"
#include "../utils/attributes.hpp"

#define DB_ENTRIES_RESERVE 100
#define ENTRY_ATTRIBUTE_RESERVE 15

enum BibType{
    // uninitialized
    BT_NONE,
    // non-BibTex
    BT_ELSE,
    // BibTex
    BT_ARTICLE,
    BT_BOOK,
    BT_BOOKLET,
    BT_CONFERENCE,
    BT_INBOOK,
    BT_INCOLLECTION,
    BT_INPROCEEDINGS,
    BT_MANUAL,
    BT_MASTERTHESIS,
    BT_PHDTHESIS,
    BT_PROCEEDINGS,
    BT_TECHREPORT,
    BT_UNPUBLISHED,
    BT_MISC,
    // arxiv entries need special treatment
    BT_ARXIV,
};

constexpr BibType getBibType(const std::string &str) noexcept {
    
    if (str=="article") return BT_ARTICLE;
    if (str=="book") return BT_BOOK;
    if (str=="booklet") return BT_BOOKLET;
    if (str=="conference") return BT_CONFERENCE;
    if (str=="inbook") return BT_INBOOK;
    if (str=="incollection") return BT_INCOLLECTION;
    if (str=="inproceedings") return BT_INPROCEEDINGS;
    if (str=="manual") return BT_MANUAL;
    if (str=="masterthesis") return BT_MASTERTHESIS;
    if (str=="phdthesis") return BT_PHDTHESIS;
    if (str=="proceedings") return BT_PROCEEDINGS;
    if (str=="techreport") return BT_TECHREPORT;
    if (str=="unpublished") return BT_UNPUBLISHED;
    if (str=="misc") return BT_MISC;

    return BT_ELSE;
}

class BibEntry{
public:
    explicit BibEntry(
        const std::string bibTypeName,
        const std::string bibKey
    );

    virtual ~BibEntry(){};

    std::string bibKey;
    std::string bibTypeName;
    std::unordered_map<std::string, std::string> attributes;
    std::unordered_map<std::string, bool> keepAttributes;

    void addAttribute(std::string tag, std::string body);

    bool keep;
    void cleanup();

    bool doiChecked;
    bool arxivChecked;
    BibType bibType;
    void findBibType() noexcept;

private:
    void checkArXiv() noexcept;
};

class SpecialEntry{
public:
    explicit SpecialEntry(
        const std::string bibTypeName,
        const std::string body
    ) : bibTypeName(bibTypeName),
        body(body){}

    virtual ~SpecialEntry(){};

    const std::string bibTypeName;
    std::string body;
};

class BibDB{
public:
    explicit BibDB();

    virtual ~BibDB(){};

    std::vector<std::unique_ptr<BibEntry>> entries;
    uint64_t numEntries;
    std::vector<std::unique_ptr<SpecialEntry>> specialEntries;
    uint64_t numSpecialEntries;

    void addEntry(std::unique_ptr<BibEntry> newEntry);
    void addSpecialEntry(std::unique_ptr<SpecialEntry> newEntry);

    void clear();
    void cleanup();
    uint64_t countKeep() const;
};
