#pragma once

#include <unordered_map>
#include <unordered_set>

#include "../database/bibfile.hpp"
#include "../utils/attributes.hpp"

class BibTexFields{
public:
    explicit BibTexFields(){}

    virtual ~BibTexFields(){};

    const std::unordered_set<std::string> keepAlways = {
        "note",
        "doi",
        "author",
        "title",
        "year",
        "url",
    };

    const std::unordered_set<std::string> keepBibTexArticle={
        "journal",
        "volume",
        "number",
        "pages",
        "month",
        "issn",
    };

    const std::unordered_set<std::string> keepBibTexBook={
        "editor",
        "publisher",
        "volume",
        "number",
        "pages",
        "series",
        "address",
        "edition",
        "month",
        "issn",
        "isbn",
    };

    const std::unordered_set<std::string> keepBibTexBooklet={
	    "howpublished",
	    "address",
        "editor",
        "publisher",
        "volume",
        "number",
        "pages",
        "series",
        "edition",
        "organization",
        "month",
        "issn",
        "isbn",
    };

    const std::unordered_set<std::string> keepBibTexConference={
        "booktitle",
        "editor",
        "volume",
        "number",
        "series",
        "pages",
        "address",
        "month",
        "organization",
        "publisher",
    };

    const std::unordered_set<std::string> keepBibTexInBook={
        "editor",
        "chapter",
        "pages",
        "publisher",
        "volume",
        "number",
        "series",
        "type",
        "address",
        "edition",
        "month",
    };

    const std::unordered_set<std::string> keepBibTexInCollection={
        "booktitle",
        "publisher",
        "editor",
        "volume",
        "number",
        "series",
        "type",
        "chapter",
        "pages",
        "address",
        "edition",
        "month",
    };

    const std::unordered_set<std::string> keepBibTexInProceedings={
        "booktitle",
        "editor",
        "volume",
        "number",
        "series",
        "pages",
        "address",
        "month",
        "organization",
        "publisher",
    };

    const std::unordered_set<std::string> keepBibTexManual={
        "organization",
        "address",
        "edition",
        "month",
    };

    const std::unordered_set<std::string> keepBibTexMasterThesis={
        "school",
        "type",
        "address",
        "month",
    };

    const std::unordered_set<std::string> keepBibTexMisc={
        "howpublished",
	    "month",
    };

    const std::unordered_set<std::string> keepBibTexPhDThesis={
        "school",
        "type",
        "address",
        "month",
    };

    const std::unordered_set<std::string> keepBibTexProceedings={
        "editor",
        "volume",
        "number",
        "series",
        "address",
        "month",
        "organization",
        "publisher",
    };

    const std::unordered_set<std::string> keepBibTexTechReport={
        "institution",
        "type",
        "number",
        "address",
        "month",
    };

    const std::unordered_set<std::string> keepBibTexUnpublished{
        "month",
    };

    const std::unordered_set<std::string> keepBibTexArXiv = {
        "eprint",
        "archiveprefix",
        "primaryclass",
    };

    const std::unordered_map<BibType,const std::unordered_set<std::string>> keepBibTexFields = {
        {BT_ARTICLE, keepBibTexArticle},
        {BT_BOOK, keepBibTexBook},
        {BT_BOOKLET, keepBibTexBooklet},
        {BT_CONFERENCE, keepBibTexConference},
        {BT_INBOOK, keepBibTexInBook},
        {BT_INCOLLECTION, keepBibTexInCollection},
        {BT_INPROCEEDINGS, keepBibTexInProceedings},
        {BT_MANUAL, keepBibTexManual},
        {BT_MASTERTHESIS, keepBibTexMasterThesis},
        {BT_MISC, keepBibTexMisc},
        {BT_PHDTHESIS, keepBibTexPhDThesis},
        {BT_PROCEEDINGS, keepBibTexProceedings},
        {BT_TECHREPORT, keepBibTexTechReport},
        {BT_UNPUBLISHED, keepBibTexUnpublished},
        {BT_ARXIV, keepBibTexArXiv},
    };
};

