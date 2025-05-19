#include <catch2/catch_test_macros.hpp>

#include "../../include/bibneat/database/bibfile.hpp"
#include "../../include/bibneat/database/parser.hpp"
#include "../../include/bibneat/recipes/fieldfilter.hpp"
#include "../../include/bibneat/utils/stringUtils.hpp"

#ifdef CMAKE
    #include "testBibfilesLoc.cmake.h"
#else
    #define TESTBIB_PATH ""
#endif

TEST_CASE("Filtering non-BibTeX attributes", "[filtering]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    Parser parser(bibDB);
    FieldFilter fieldFilter(bibDB);
    const BibTexFields BTFields = BibTexFields();

    SECTION("Removing fields Springer"){
        REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/extra.bib"));
        CHECK(bibDB->entries.size() == 1);
        CHECK(bibDB->entries.front()->bibKey == "Philips_2022");
        CHECK(bibDB->entries.front()->bibType == BT_ARTICLE);
        CHECK(bibDB->entries.front()->attributes.contains("abstract"));
        CHECK(bibDB->entries.front()->attributes.contains("date"));
        fieldFilter.keepBibTex();
        CHECK_FALSE(bibDB->entries.front()->keepAttributes.at("abstract"));
        CHECK_FALSE(bibDB->entries.front()->keepAttributes.at("date"));
        const std::unordered_set<std::string> keepFields = BTFields.keepBibTexFields.at(bibDB->entries.front()->bibType);
        for(auto& [key, keep]: bibDB->entries.front()->keepAttributes){
            CHECK(keep == (BTFields.keepAlways.contains(key)||keepFields.contains(key)));
        }
    }

    SECTION("Keeping arxiv fields"){
        REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/arxiv.bib"));
        CHECK(bibDB->entries.size() == 3);
        CHECK(bibDB->entries.front()->bibKey == "vonneumann2010proofergodictheoremhtheorem");
        CHECK(bibDB->entries.front()->bibType == BT_ARXIV);
        CHECK(bibDB->entries.front()->attributes.contains("eprint"));
        CHECK(bibDB->entries.front()->attributes.contains("archiveprefix"));
        CHECK(bibDB->entries.front()->attributes.contains("primaryclass"));
        CHECK(bibDB->entries.front()->attributes.contains("test"));
        fieldFilter.keepBibTex();
        CHECK(bibDB->entries.front()->attributes.contains("eprint"));
        CHECK(bibDB->entries.front()->attributes.contains("archiveprefix"));
        CHECK(bibDB->entries.front()->attributes.contains("primaryclass"));
        CHECK_FALSE(bibDB->entries.front()->keepAttributes.at("test"));
        const std::unordered_set<std::string> keepFields = BTFields.keepBibTexFields.at(bibDB->entries.front()->bibType);
        for(auto& [key, keep]: bibDB->entries.front()->keepAttributes){
            CHECK(keep == (BTFields.keepAlways.contains(key)||keepFields.contains(key)));
        }
    }

}