#include <catch2/catch_test_macros.hpp>

#include "../../include/bibneat/database/bibfile.hpp"
#include "../../include/bibneat/database/parser.hpp"
#include "../../include/bibneat/recipes/apicalls.hpp"
#include "../../include/bibneat/utils/stringUtils.hpp"

TEST_CASE("Testing arxiv bibgrab", "[api]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    ApiCaller apiCaller(bibDB, 30L, 45L);

    const std::string arxivId = "1003.2133";

    SECTION("No replace with DOI"){
        REQUIRE(apiCaller.getArXivImmediate(arxivId, false));
        REQUIRE(bibDB->entries.size()==1);
        CHECK(bibDB->entries.front()->bibKey=="vonneumann2010proofergodictheoremhtheorem");
        REQUIRE(bibDB->entries.front()->attributes.contains("author"));
        CHECK(bibDB->entries.front()->attributes.at("author")=="John von Neumann");
        REQUIRE(bibDB->entries.front()->attributes.contains("eprint"));
        CHECK(bibDB->entries.front()->attributes.at("eprint")=="1003.2133");
    }

    SECTION("Replace with DOI"){
        REQUIRE(apiCaller.getArXivImmediate(arxivId, true));
        CHECK(bibDB->entries.front()->bibKey=="von_Neumann_2010");
        REQUIRE(bibDB->entries.front()->attributes.contains("author"));
        CHECK(bibDB->entries.front()->attributes.at("author")=="von Neumann, J.");
        REQUIRE(bibDB->entries.front()->attributes.contains("journal"));
        CHECK(bibDB->entries.front()->attributes.at("journal")=="The European Physical Journal H");
    }
}

TEST_CASE("Testing doi bibgrab", "[api]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    ApiCaller apiCaller(bibDB, 30L, 45L);

    const std::string doi = "10.1140/epjh/e2010-00008-5";

    REQUIRE(apiCaller.getDOIImmediate(doi));
    REQUIRE(bibDB->entries.size()==1);
    CHECK(bibDB->entries.front()->bibKey=="von_Neumann_2010");
    REQUIRE(bibDB->entries.front()->attributes.contains("author"));
    CHECK(bibDB->entries.front()->attributes.at("author")=="von Neumann, J.");
    REQUIRE(bibDB->entries.front()->attributes.contains("journal"));
    CHECK(bibDB->entries.front()->attributes.at("journal")=="The European Physical Journal H");

}