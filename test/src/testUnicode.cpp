#include <catch2/catch_test_macros.hpp>

#include "../../include/bibneat/database/bibfile.hpp"
#include "../../include/bibneat/database/parser.hpp"
#include "../../include/bibneat/uni/fieldnormalization.hpp"
#include "../../include/bibneat/utils/stringUtils.hpp"

#ifdef CMAKE
    #include "testBibfilesLoc.cmake.h"
#else
    #define TESTBIB_PATH ""
#endif

TEST_CASE("Unicode NFC normalization", "[unicode]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    Parser parser(bibDB);
    FieldNormalizer fieldNorm(bibDB);


    SECTION("Accents"){
        CHECK(parser.parse(TESTBIB_PATH"testBibfiles/nonAscii.bib"));
        CHECK(bibDB->entries.size() == 4);
        REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "Über"));
        REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "Mądzik"));
        REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "Körper"));
        fieldNorm.NFCNormalize();
        REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "Über"));
        REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "Mądzik"));
        REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "Körper"));
    }

    SECTION("Special characters"){
        CHECK(parser.parse(TESTBIB_PATH"testBibfiles/special.bib"));
        CHECK(bibDB->entries.size() == 4);
        REQUIRE(bibDB->entries.at(0)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("author"), "Σοφοκλής"));
        REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "Œdipus"));
        REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "Æschylus"));
        REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "A ﬁne poem on ﬂowers"));
        REQUIRE(bibDB->entries.at(3)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(3)->attributes.at("title"), "A ﬅunning ſcene"));
        fieldNorm.NFCNormalize();
        REQUIRE(bibDB->entries.at(0)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("author"), "Σοφοκλής"));
        REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "Œdipus"));
        REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "Æschylus"));
        REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "A ﬁne poem on ﬂowers"));
        REQUIRE(bibDB->entries.at(3)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(3)->attributes.at("title"), "A ﬅunning ſcene"));
    }

}

TEST_CASE("Unicode NFKC normalization", "[unicode]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    Parser parser(bibDB);
    FieldNormalizer fieldNorm(bibDB);

    SECTION("Accents"){
        CHECK(parser.parse(TESTBIB_PATH"testBibfiles/nonAscii.bib"));
        CHECK(bibDB->entries.size() == 4);
        REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "Über"));
        REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "Mądzik"));
        REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "Körper"));
        fieldNorm.NFKCNormalize();
        REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "Über"));
        REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "Mądzik"));
        REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "Körper"));
    }

    SECTION("Special characters"){
        CHECK(parser.parse(TESTBIB_PATH"testBibfiles/special.bib"));
        CHECK(bibDB->entries.size() == 4);
        REQUIRE(bibDB->entries.at(0)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("author"), "Σοφοκλής"));
        REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "Œdipus"));
        REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "Æschylus"));
        REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "A ﬁne poem on ﬂowers"));
        REQUIRE(bibDB->entries.at(3)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(3)->attributes.at("title"), "A ﬅunning ſcene"));
        fieldNorm.NFKCNormalize();
        REQUIRE(bibDB->entries.at(0)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("author"), "Σοφοκλής"));
        REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "Œdipus"));
        REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "Æschylus"));
        REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "A fine poem on flowers"));
        REQUIRE(bibDB->entries.at(3)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(3)->attributes.at("title"), "A stunning scene"));
    }

}

TEST_CASE("Unicode to latex conversion", "[unicode]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    Parser parser(bibDB);
    FieldNormalizer fieldNorm(bibDB);

    SECTION("Accents"){
        CHECK(parser.parse(TESTBIB_PATH"testBibfiles/nonAscii.bib"));
        CHECK(bibDB->entries.size() == 4);
        REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "Über"));
        REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "Mądzik"));
        REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "Körper"));
        fieldNorm.uni2latex();
        REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "\\\"{U}ber"));
        REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "M\\k{a}dzik"));
        REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "K\\\"{o}rper"));
    }

    SECTION("Special characters"){
        CHECK(parser.parse(TESTBIB_PATH"testBibfiles/special.bib"));
        CHECK(bibDB->entries.size() == 4);
        REQUIRE(bibDB->entries.at(0)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("author"), "Σοφοκλής"));
        REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "Œdipus"));
        REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "Æschylus"));
        REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "A ﬁne poem on ﬂowers"));
        REQUIRE(bibDB->entries.at(3)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(3)->attributes.at("title"), "A ﬅunning ſcene"));
        fieldNorm.uni2latex();
        REQUIRE(bibDB->entries.at(0)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("author"), "Σοφοκλ\\'{η}ς"));
        REQUIRE(bibDB->entries.at(0)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(0)->attributes.at("title"), "{\\OE}dipus Rex"));
        REQUIRE(bibDB->entries.at(1)->attributes.contains("author"));
        CHECK(string_contains(bibDB->entries.at(1)->attributes.at("author"), "{\\AE}schylus"));
        REQUIRE(bibDB->entries.at(2)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(2)->attributes.at("title"), "A ﬁne poem on ﬂowers"));
        REQUIRE(bibDB->entries.at(3)->attributes.contains("title"));
        CHECK(string_contains(bibDB->entries.at(3)->attributes.at("title"), "A ﬅunning ſcene"));
    }

}


TEST_CASE("Unicode preamble", "[unicode]"){
    std::shared_ptr<BibDB> bibDB = std::make_shared<BibDB>();
    Parser parser(bibDB);
    FieldNormalizer fieldNorm(bibDB);
    fieldNorm.addUTF8Preamble();
    CHECK(bibDB->entries.size() == 0);
    CHECK(bibDB->specialEntries.size() == 1);
    CHECK(bibDB->specialEntries.front()->bibTypeName=="preamble");
}