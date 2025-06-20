#include <catch2/catch_test_macros.hpp>
#include <stdint.h>

#include "../../include/bibneat/database/bibfile.hpp"
#include "../../include/bibneat/database/parser.hpp"
#include "../../include/bibneat/utils/stringUtils.hpp"

#ifdef CMAKE
    #include "testBibfilesLoc.cmake.h"
#else
    #define TESTBIB_PATH ""
#endif


TEST_CASE("Testing auto-incrementing index", "[parsing]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    Parser parser(bibDB);

    SECTION("Multiple-entry file"){
        REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/multiple.bib"));
        CHECK(bibDB->entries.size() == 3);
        CHECK(bibDB->numEntries == 3);
        const uint64_t startIdx = bibDB->entries.front()->index;
        for(uint64_t idx=0; idx<bibDB->numEntries; idx++){
            CHECK(bibDB->entries.at(idx)->index == idx + startIdx);
        }
    }

    SECTION("Multiple files"){
        REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/multiple.bib"));
        CHECK(bibDB->entries.size() == 3);
        CHECK(bibDB->numEntries == 3);
        REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/single.bib"));
        CHECK(bibDB->entries.size() == 4);
        CHECK(bibDB->numEntries == 4);
        const uint64_t startIdx = bibDB->entries.front()->index;
        for(uint64_t idx=0; idx<bibDB->numEntries; idx++){
            CHECK(bibDB->entries.at(idx)->index == idx + startIdx);
        }
    }

}