#include <gtest/gtest.h>
#include "helper.hpp"
#include <cpplibxml2.hpp>

#include <fstream>

static const std::filesystem::path exampleFile{"testData/example.xml"};
static const std::filesystem::path emptyFile{"testData/empty.xml"};

TEST(DocClass, parseFile) {
    ASSERT_TRUE(std::filesystem::exists(exampleFile));
    ASSERT_NO_THROW(std::ignore = cpplibxml2::Doc::parseFile(exampleFile));
}

TEST(DocClass, parseEmptyFilePath) {
    auto DocResult = cpplibxml2::Doc::parseFile("");
    ASSERT_FALSE(DocResult);
    ASSERT_STREQ(DocResult.error().what(), "Document don't exist.");
}

TEST(DocClass, parseEmptyFile) {
    ASSERT_TRUE(std::filesystem::exists(emptyFile));
    auto DocResult = cpplibxml2::Doc::parseFile(emptyFile);
    ASSERT_FALSE(DocResult);
    ASSERT_STREQ(DocResult.error().what(), "Document not parsed successfully.");
}

TEST(DocClass, MoveOperators) {
    ASSERT_TRUE(std::filesystem::exists(exampleFile));
    auto doc = cpplibxml2::Doc::parseFile(exampleFile);
    auto doc2{std::move(doc)};
}

TEST(DocClass, MoveAssignmentOperator) {
    constexpr std::string_view xml = "<root><child>data</child></root>";
    constexpr std::string_view xml2 = "<root><child>data2</child></root>";
    auto doc1 = cpplibxml2::Doc::parse(xml);
    auto doc2 = cpplibxml2::Doc::parse(xml2);

    doc2 = std::move(doc1);
}

TEST(DocClass, parse) {
    ASSERT_TRUE(std::filesystem::exists(exampleFile));
    auto iFile = std::ifstream{exampleFile};
    ASSERT_TRUE(iFile.is_open());
    std::stringstream buffer;
    buffer << iFile.rdbuf();
    iFile.close();

    ASSERT_NO_THROW(std::ignore = cpplibxml2::Doc::parse(buffer.str()));
}

TEST(DocClass, parseEmptyString) {
    auto DocResult = cpplibxml2::Doc::parse("");
    ASSERT_FALSE(DocResult);
    ASSERT_STREQ(DocResult.error().what(), "Document is empty.");
}

TEST(DocClass, parseRandomString) {
    auto DocResult = cpplibxml2::Doc::parse(generateRandomString(256));
    ASSERT_FALSE(DocResult);
    ASSERT_STREQ(DocResult.error().what(), "Document not parsed successfully.");
}

TEST(DocClass, dumpXML)
{
    constexpr auto orgXML = std::string_view{"<root><child>data</child></root>"};
    const auto doc = cpplibxml2::Doc::parse(orgXML);
    ASSERT_TRUE(doc);
    const auto xml = doc.value().dump();
    ASSERT_TRUE(xml);
    ASSERT_STREQ(xml.value().data(), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root><child>data</child></root>\n");
}

TEST(DocClass, dumpFormatedXML)
{
    constexpr auto orgXML = std::string_view{"<root><child>data</child></root>"};
    const auto doc = cpplibxml2::Doc::parse(orgXML);
    ASSERT_TRUE(doc);
    const auto xml = doc.value().dump(true);
    ASSERT_TRUE(xml);
    ASSERT_STREQ(xml.value().data(), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\n  <child>data</child>\n</root>\n");
}

TEST(DocClass, dumpFormatedXMLWithSpecialCharsToISO)
{
    constexpr auto orgXML = std::string_view{"<root><child>你好, мир, مرحبا, नमस्ते 🌍, äöüßÄÖÜ€</child></root>"};
    const auto doc = cpplibxml2::Doc::parse(orgXML);
    ASSERT_TRUE(doc);
    const auto xml = doc.value().dump(true, cpplibxml2::Format::ISO_8859_1);
    ASSERT_TRUE(xml);
    ASSERT_STREQ(xml.value().data(), "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<root>\n  <child>&#20320;&#22909;, &#1084;&#1080;&#1088;, &#1605;&#1585;&#1581;&#1576;&#1575;, &#2344;&#2350;&#2360;&#2381;&#2340;&#2375; &#127757;, \xE4\xF6\xFC\xDF\xC4\xD6\xDC&#8364;</child>\n</root>\n");
}
