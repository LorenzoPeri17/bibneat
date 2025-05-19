#include <catch2/catch_test_macros.hpp>

#include "../../include/bibneat/database/bibfile.hpp"
#include "../../include/bibneat/database/parser.hpp"
#include "../../include/bibneat/utils/stringUtils.hpp"

#ifdef CMAKE
    #include "testBibfilesLoc.cmake.h"
#else
    #define TESTBIB_PATH ""
#endif

TEST_CASE("Testing the bibtex parser", "[parsing]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    Parser parser(bibDB);

    SECTION("Testing parsing bib files"){

        SECTION("Single-entry file"){
            REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/single.bib"));
            CHECK(bibDB->entries.size() == 1);
            CHECK(bibDB->entries.front()->bibKey == "Newman_1942");
            CHECK(bibDB->entries.front()->bibType == BT_ARTICLE);
        }

        SECTION("Multiple-entry file"){
            REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/multiple.bib"));
            CHECK(bibDB->entries.size() == 3);
            CHECK(bibDB->entries.at(0)->bibKey == "Talbot_1834");
            CHECK(bibDB->entries.at(1)->bibKey == "Dirac_1925");
            CHECK(bibDB->entries.at(2)->bibKey == "Kubo_1966");
            REQUIRE(bibDB->entries.at(1)->attributes.contains("title"));
            CHECK(bibDB->entries.at(1)->attributes.at("title")=="The Fundamental Equations of Quantum Mechanics");
        }

        SECTION("Multiple files"){
            REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/multiple.bib"));
            CHECK(bibDB->entries.size() == 3);
            REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/single.bib"));
            CHECK(bibDB->entries.size() == 4);
        }

        SECTION("Curly braces and one-word attributes"){
            REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/groups.bib"));
            CHECK(bibDB->entries.size() == 1);
            CHECK(bibDB->entries.front()->bibKey == "Penrose_Rindler_1984");
            REQUIRE(bibDB->entries.front()->attributes.contains("year"));
            CHECK(bibDB->entries.front()->attributes.at("year")=="1984");
            REQUIRE(bibDB->entries.front()->attributes.contains("publisher"));
            CHECK(bibDB->entries.front()->attributes.at("publisher")=="{Cambridge University} Press");
        }


        SECTION("Entries without @ are junk"){
            REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/bad.bib"));
            CHECK(bibDB->entries.size() == 2);
            CHECK(bibDB->entries.at(0)->bibKey == "Talbot_1834");
            CHECK(bibDB->entries.at(1)->bibKey == "Kubo_1966");
            REQUIRE(bibDB->entries.at(1)->attributes.contains("title"));
            CHECK(bibDB->entries.at(1)->attributes.at("title")=="The fluctuation-dissipation theorem");
        }

        SECTION("Quotes and string concatenation with #"){
            REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/quotes.bib"));
            CHECK(bibDB->entries.size() == 2);
            CHECK(bibDB->entries.at(0)->bibKey == "patashnik-bibtexing");
            CHECK(bibDB->entries.at(1)->bibKey == "mrx05");
            REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
            CHECK(bibDB->entries.at(0)->attributes.at("title")=="BIBTEXing");
            REQUIRE(bibDB->entries.at(1)->attributes.contains("publisher"));
            CHECK(bibDB->entries.at(1)->attributes.at("publisher")=="nobody");

        }

        SECTION("Preamble entries"){
            REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/preamble.bib"));
            CHECK(bibDB->entries.size() == 0);
            CHECK(bibDB->specialEntries.size() == 1);
            CHECK(bibDB->specialEntries.front()->bibTypeName=="preamble");
        }

        SECTION("non-ascii characters"){
            REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/nonAscii.bib"));
            CHECK(bibDB->entries.size() == 4);
            CHECK(bibDB->entries.at(0)->bibKey == "Einstein_1905_Über");
            CHECK(bibDB->entries.at(1)->bibKey == "Philips_2022");            
            CHECK(bibDB->entries.at(2)->bibKey == "Einstein_1905_Elektrodynamik");
            CHECK(bibDB->entries.at(3)->bibKey == "Peri_2023");
            REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
            CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "Über"));
            REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
            CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "Mądzik"));
            REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
            CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "Körper"));
            REQUIRE(bibDB->entries.at(3)->attributes.contains("title"));
            CHECK(string_contains(bibDB->entries.at(3)->attributes.at("title"), "3ω"));
        }

        SECTION("Recognizing arXiv entries"){
            REQUIRE(parser.parse(TESTBIB_PATH"testBibfiles/arxiv.bib"));
            CHECK(bibDB->entries.size() == 3);
            CHECK(bibDB->entries.at(0)->bibKey == "vonneumann2010proofergodictheoremhtheorem");
            CHECK(bibDB->entries.at(0)->bibTypeName == "misc");
            CHECK(bibDB->entries.at(0)->bibType == BT_ARXIV);
            CHECK(bibDB->entries.at(1)->bibKey == "imamura1999stringjunctionsdualsheterotic");
            CHECK(bibDB->entries.at(1)->bibTypeName == "misc");
            CHECK(bibDB->entries.at(1)->bibType == BT_ARXIV);
            CHECK(bibDB->entries.at(2)->bibKey == "preskill2023quantumcomputing40years");
            CHECK(bibDB->entries.at(2)->bibTypeName == "misc");
            CHECK(bibDB->entries.at(2)->bibType == BT_ARXIV);
        }

        SECTION("Testing parsing bib entries from strings"){

            SECTION("Testing response from doi.org api"){
                const std::string apiBib = "@article{Dirac_1925,\n"
                "title={The Fundamental Equations of Quantum Mechanics},\n"
                "author={Dirac, P. A. M.},\n"
                "year={1925},\n"
                "journal={Proceedings of the Royal Society of London. Series A, Containing Papers of a Mathematical and Physical Character},\n"
                "pages={642–653},\n"
                "publisher={The Royal Society},\n"
                "issn={0950-1207},\n"
                "number={752},\n"
                "volume={109},\n"
                "}";
                const bool success = parser.parseString(apiBib);
                CHECK(bibDB->entries.size() == 1);
                CHECK(bibDB->entries.front()->bibKey == "Dirac_1925");
            }

            SECTION("Testing response from arxiv.org api"){
                const std::string apiBib = "@misc{preskill2023quantumcomputing40years,\n"
                "      title={Quantum computing 40 years later}, \n"
                "      author={John Preskill},\n"
                "      year={2023},\n"
                "      eprint={2106.10522},\n"
                "      archivePrefix={arXiv},\n"
                "      primaryClass={quant-ph},\n"
                "      url={https://arxiv.org/abs/2106.10522}, \n"
                "}";
                const bool success = parser.parseString(apiBib);
                CHECK(bibDB->entries.size() == 1);
                CHECK(bibDB->entries.front()->bibKey == "preskill2023quantumcomputing40years");
            }


        }
    }
}