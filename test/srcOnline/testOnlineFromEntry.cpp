#include <catch2/catch_test_macros.hpp>

#include "../../include/bibneat/database/bibfile.hpp"
#include "../../include/bibneat/database/parser.hpp"
#include "../../include/bibneat/recipes/apicalls.hpp"
#include "../../include/bibneat/utils/stringUtils.hpp"

TEST_CASE("Testing arxiv bibneat", "[api]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    Parser parser(bibDB);
    ApiCaller apiCaller(bibDB, 30L, 45L);

    const std::string arxivEntry = "@misc{vonneumann_arxiv_incomplete,\n"
    "url={https://arxiv.org/abs/1003.2133},\n"
    "}";

    SECTION("No replacement"){
        REQUIRE(parser.parseString(arxivEntry));
        CHECK(bibDB->entries.size() == 1);
        CHECK(bibDB->entries.front()->bibType == BT_ARXIV);
        CHECK(bibDB->entries.front()->bibKey == "vonneumann_arxiv_incomplete");
        apiCaller.checkArXiv(false, false);
        CHECK(bibDB->entries.front()->keep == true);
        CHECK(bibDB->entries.front()->bibKey == "vonneumann_arxiv_incomplete");
        CHECK_FALSE(bibDB->entries.front()->attributes.contains("author"));
    }

    SECTION("Replacement with arxiv"){
        REQUIRE(parser.parseString(arxivEntry));
        REQUIRE(bibDB->entries.size() == 1);
        CHECK(bibDB->entries.front()->bibType == BT_ARXIV);
        CHECK(bibDB->entries.front()->bibKey == "vonneumann_arxiv_incomplete");
        apiCaller.checkArXiv(true, false);
        REQUIRE(bibDB->entries.size() == 1);
        CHECK(bibDB->entries.front()->keep == true);
        CHECK(bibDB->entries.front()->bibKey == "vonneumann_arxiv_incomplete");
        REQUIRE(bibDB->entries.front()->attributes.contains("author"));
        CHECK(bibDB->entries.front()->attributes.at("author")=="John von Neumann");
        REQUIRE(bibDB->entries.front()->attributes.contains("eprint"));
        CHECK(bibDB->entries.front()->attributes.at("eprint")=="1003.2133");
    }

    SECTION("Replacement with doi"){
        REQUIRE(parser.parseString(arxivEntry));
        REQUIRE(bibDB->entries.size() == 1);
        CHECK(bibDB->entries.front()->bibType == BT_ARXIV);
        CHECK(bibDB->entries.front()->bibKey == "vonneumann_arxiv_incomplete");
        apiCaller.checkArXiv(true, true);
        REQUIRE(bibDB->entries.size() == 1);
        CHECK(bibDB->entries.front()->keep == true);
        CHECK(bibDB->entries.front()->bibKey == "vonneumann_arxiv_incomplete");
        REQUIRE(bibDB->entries.front()->attributes.contains("author"));
        CHECK(bibDB->entries.front()->attributes.at("author")=="von Neumann, J.");
        REQUIRE(bibDB->entries.front()->attributes.contains("journal"));
        CHECK(bibDB->entries.front()->attributes.at("journal")=="The European Physical Journal H");
    }

}


TEST_CASE("Testing DOI bibneat", "[api]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    Parser parser(bibDB);
    ApiCaller apiCaller(bibDB, 30L, 45L);

    const std::string doiEntry = "@article{vonneumann_doi_incomplete,\n"
    "doi={10.1140/epjh/e2010-00008-5},\n"
    "}";

    SECTION("No replacement"){
        REQUIRE(parser.parseString(doiEntry));
        CHECK(bibDB->entries.size() == 1);
        CHECK(bibDB->entries.front()->bibType == BT_ARTICLE);
        CHECK(bibDB->entries.front()->bibKey == "vonneumann_doi_incomplete");
        apiCaller.checkDOI(false);
        CHECK(bibDB->entries.size() == 1);
        CHECK(bibDB->entries.front()->bibKey == "vonneumann_doi_incomplete");
        CHECK_FALSE(bibDB->entries.front()->attributes.contains("author"));

    }

    SECTION("Replacement with doi"){
        REQUIRE(parser.parseString(doiEntry));
        CHECK(bibDB->entries.size() == 1);
        CHECK(bibDB->entries.front()->bibType == BT_ARTICLE);
        CHECK(bibDB->entries.front()->bibKey == "vonneumann_doi_incomplete");
        apiCaller.checkDOI(true);
        CHECK(bibDB->entries.size() == 1);
        CHECK(bibDB->entries.front()->keep == true);
        CHECK(bibDB->entries.front()->bibKey == "vonneumann_doi_incomplete");
        REQUIRE(bibDB->entries.front()->attributes.contains("author"));
        CHECK(bibDB->entries.front()->attributes.at("author")=="von Neumann, J.");
        REQUIRE(bibDB->entries.front()->attributes.contains("journal"));
        CHECK(bibDB->entries.front()->attributes.at("journal")=="The European Physical Journal H");
    }

}