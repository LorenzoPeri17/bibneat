#include <catch2/catch_test_macros.hpp>

#include "../../include/bibneat/database/bibfile.hpp"
#include "../../include/bibneat/database/parser.hpp"
#include "../../include/bibneat/recipes/apicalls.hpp"
#include "../../include/bibneat/utils/stringUtils.hpp"

#ifdef CMAKE
    #include "testBibfilesLoc.cmake.h"
#else
    #define TESTBIB_PATH ""
#endif

class ApiCallerTester: protected ApiCaller {
public:
    explicit ApiCallerTester (
        std::shared_ptr<BibDB> bibDB
        // we have no plan of making requests
        ): ApiCaller(bibDB, 0L, 0L){}

    const std::string getPrepArXivFromEntry(const BibEntry* entry){
        REQUIRE(entry->bibType == BT_ARXIV);
        return this->getArXivId(entry);
    }

    const std::string getPrepDOIFromEntry(const BibEntry* entry){
        REQUIRE(this->hasDOI(entry));
        const std::string doi = entry->attributes.at("doi");
        return this->prepDOI(doi);
    }

    const std::string getPrepArXiv(const std::string arxiv){
        return this->prepArXiv(arxiv);
    }

    const std::string getPrepDOI(const std::string doi){
        return this->prepDOI(doi);
    }

};

TEST_CASE("Extracting identifiers from entries", "[api]"){

    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    Parser parser(bibDB);
    ApiCallerTester apiCaller(bibDB);

    SECTION("Arxiv IDs"){
        REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/arxiv.bib"));
        CHECK(bibDB->entries.size() == 3);
        CHECK(apiCaller.getPrepArXivFromEntry(bibDB->entries.at(0).get()) == "1003.2133");
        CHECK(apiCaller.getPrepArXivFromEntry(bibDB->entries.at(1).get()) == "hep-th/9901001");
        CHECK(apiCaller.getPrepArXivFromEntry(bibDB->entries.at(2).get()) == "2106.10522");
    }

    SECTION("DOIs"){
        REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/arxiv.bib"));
        CHECK(bibDB->entries.size() == 3);
        CHECK(apiCaller.getPrepDOIFromEntry(bibDB->entries.at(0).get()) == "10.1140/epjh/e2010-00008-5");
        REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/groups.bib"));
        CHECK(bibDB->entries.size() == 4);
        CHECK(apiCaller.getPrepDOIFromEntry(bibDB->entries.back().get()) == "10.1017/CBO9780511564048");
        REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/extra.bib"));
        CHECK(bibDB->entries.size() == 5);
        CHECK(apiCaller.getPrepDOIFromEntry(bibDB->entries.back().get()) == "10.1038/s41586-022-05117-x");
    }
}

static inline void test_arxiv(ApiCallerTester *apiCaller, std::string const& id){
    const std::vector<std::string> variants = {
            id,
            "arXiv:"+id,
            "arXiv."+id,
            "arxiv:"+id,
            "arxiv."+id,
            "https://arxiv.org/abs/"+id,
            "https://arxiv.org/pdf/"+id,
        };
    for (auto& v : variants){
        CHECK(apiCaller->getPrepArXiv(v) == id);
    }
}

TEST_CASE("Testing my beautiful arxiv regex", "[api]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    ApiCallerTester apiCaller(bibDB);

    const std::vector<std::string> ids = {
        "2106.10522",
        "1003.2133",
        "hep-th/9901001",
        "math.GT/0309136",
        "hep-th/9802150",
        "astro-ph/0002188",
        "1707.04169",
        "math/0606579",
        "1211.4914"
    };

    for (auto& id: ids){
        test_arxiv(&apiCaller, id);
    }

}

static inline void test_doi(ApiCallerTester *apiCaller, std::string const& doi){
    const std::vector<std::string> variants = {
            doi,
            "http://doi.org/"+doi,
            "https://doi.org/"+doi,
            "http://dx.doi.org/"+doi,
            "https://dx.doi.org/"+doi,
        };
    for (auto& v : variants){
        CHECK(apiCaller->getPrepDOI(v) == doi);
    }
}

TEST_CASE("Testing my beautiful doi regex", "[api]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    ApiCallerTester apiCaller(bibDB);

    const std::vector<std::string> dois = {
        "10.1093/ajae/aaq063",
        "10.1371/journal.pgen.1001111",
        "10.1111/dome.12082",
        "10.1080/10509585.2015.1092083",
        "10.3897/rio.7.e67379",
        "10.5438/9te8-5h68",
        "10.4233/UUID:6D192FE2-DE18-4556-873A-D3CD56AB96A6",
        "10.5555/KVTD-VPWM",
        "10.1000/182",
        "10.5281/ZENODO.31780"
    };

    for (auto& doi: dois){
        test_doi(&apiCaller, doi);
    }
}