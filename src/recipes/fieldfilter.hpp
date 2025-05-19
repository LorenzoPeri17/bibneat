#pragma once

/**********************************************************
*                                                         *
* Inspired by the wonderful bibtool:                      *
* https://www.gerd-neugebauer.de/software/TeX/BibTool/en/ *
*                                                         *
**********************************************************/

#include <string>
#include <unordered_set>

#include "../utils/debug.hpp"

#include "../database/bibfile.hpp"

#include "keepbibtex.hpp"

class FieldFilter{
public:
    explicit FieldFilter(
        std::shared_ptr<BibDB> bibDB
        ): bibDB(bibDB){}

    virtual ~FieldFilter(){}

    void keepBibTex() noexcept;

private:
    std::shared_ptr<BibDB> bibDB;
    const BibTexFields BTFields = BibTexFields();
};
