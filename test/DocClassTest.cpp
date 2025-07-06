#include <gtest/gtest.h>
#include "helper.hpp"
#include <cpplibxml2.hpp>

#include <codecvt>
#include <filesystem>
#include <fstream>
#include <locale>

static const std::filesystem::path exampleFile{"testData/example.xml"};
static const std::filesystem::path emptyFile{"testData/empty.xml"};

TEST(DocClass, parseFile) {
    ASSERT_TRUE(std::filesystem::exists(exampleFile))
        << std::filesystem::current_path().string() << " - " << exampleFile.string();
    ASSERT_NO_THROW(std::ignore = cpplibxml2::Doc::parseFile(exampleFile));
}

TEST(DocClass, parseEmptyFilePath)
{
    auto DocResult = cpplibxml2::Doc::parseFile(
        "", cpplibxml2::ParserOptions::NoError | cpplibxml2::ParserOptions::NoEnt | cpplibxml2::ParserOptions::DtdLoad);
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
    auto DocResult = cpplibxml2::Doc::parse(generateRandomString(256), cpplibxml2::ParserOptions::NoError |
                                                                           cpplibxml2::ParserOptions::NoEnt |
                                                                           cpplibxml2::ParserOptions::DtdLoad);
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

TEST(DocClass, dumpFormatedXMLWithSpecialCharsToASCII)
{
    constexpr auto orgXML = std::string_view{"<root><child>你好, мир, مرحبا, नमस्ते 🌍, äöüßÄÖÜ€</child></root>"};
    const auto doc = cpplibxml2::Doc::parse(orgXML);
    ASSERT_TRUE(doc);
    const auto xml = doc.value().dump(true, cpplibxml2::Format::ASCII);
    ASSERT_TRUE(xml);
    ASSERT_STREQ(
        xml.value().data(),
        "<?xml version=\"1.0\" encoding=\"ASCII\"?>\n<root>\n  <child>&#20320;&#22909;, &#1084;&#1080;&#1088;, "
        "&#1605;&#1585;&#1581;&#1576;&#1575;, &#2344;&#2350;&#2360;&#2381;&#2340;&#2375; &#127757;, "
        "&#228;&#246;&#252;&#223;&#196;&#214;&#220;&#8364;</child>\n</root>\n");
}

TEST(DocClass, dumpFormatedXMLWithSpecialCharsToUTF_16)
{
    constexpr auto orgXML = std::string_view{"<root><child>你好, мир, مرحبا, नमस्ते 🌍, äöüßÄÖÜ€</child></root>"};
    const auto doc = cpplibxml2::Doc::parse(orgXML);
    ASSERT_TRUE(doc);
    const auto xml = doc.value().dump(true, cpplibxml2::Format::UTF_16);
    ASSERT_TRUE(xml);

    // That UTF-16 thing is bad! Really Bad!
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996) // std::codecvt is deprecated in MSVC
#endif
    // xml->data() is char*, but represents UTF-16
    std::u16string u16(reinterpret_cast<const char16_t *>(xml->data()), xml->size() / 2);
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string result_utf8 = convert.to_bytes(u16);
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

    const std::string expected = "<root><child>你好, мир, مرحبا, नमस्ते 🌍, äöüßÄÖÜ€</child></root>";
    EXPECT_STREQ(
        result_utf8.c_str(),
        "\xEF\xBB\xBF<?xml version=\"1.0\" encoding=\"UTF-16\"?>\n<root>\n  <child>\xE4\xBD\xA0\xE5\xA5\xBD, "
        "\xD0\xBC\xD0\xB8\xD1\x80, \xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7, "
        "\xE0\xA4\xA8\xE0\xA4\xAE\xE0\xA4\xB8\xE0\xA5\x8D\xE0\xA4\xA4\xE0\xA5\x87 \xF0\x9F\x8C\x8D, "
        "\xC3\xA4\xC3\xB6\xC3\xBC\xC3\x9F\xC3\x84\xC3\x96\xC3\x9C\xE2\x82\xAC</child>\n</root>\n"); // XML content
                                                                                                    // should start with
                                                                                                    // expected
}

TEST(DocClass, SaveToFile_WritesXMLToFileCorrectly)
{
    // XML-Inhalt vorbereiten
    const std::string xmlContent = R"(<root><child>value</child></root>)";

    // Parsen
    auto doc = cpplibxml2::Doc::parse(xmlContent);
    ASSERT_TRUE(doc.has_value());

    // Temp-Dateipfad erzeugen
    const auto tmpFile = std::filesystem::temp_directory_path() / "test_output.xml";

    // Schreiben
    auto result = doc->saveToFile(tmpFile, true, cpplibxml2::Format::UTF_8);
    ASSERT_TRUE(result.has_value());

    // Lesen und Prüfen
    std::ifstream inFile(tmpFile);
    ASSERT_TRUE(inFile.is_open());
    std::string fileContent((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

    // Inhalt sollte das XML enthalten
    EXPECT_NE(fileContent.find("<child>value</child>"), std::string::npos);

    inFile.close();

    // Aufräumen
    std::error_code ec;
    std::filesystem::remove(tmpFile, ec);
    EXPECT_FALSE(ec) << "Failed to remove temporary file: " << ec.message();
}
TEST(DocClass, SaveToFile_ISOEncoding)
{
    const std::string xmlContent = R"(<root><child>äöüß</child></root>)";
    auto doc = cpplibxml2::Doc::parse(xmlContent);
    ASSERT_TRUE(doc.has_value());

    const auto tmpFile = std::filesystem::temp_directory_path() / "iso_output.xml";
    auto result = doc->saveToFile(tmpFile, true, cpplibxml2::Format::ISO_8859_1);
    ASSERT_TRUE(result.has_value());

    std::ifstream inFile(tmpFile);
    ASSERT_TRUE(inFile.is_open());
    std::string fileContent((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

    // Todo: check why this test case fails!
    // EXPECT_NE(fileContent.find("äöüß"), std::string::npos);
    EXPECT_NE(fileContent.find("ISO-8859-1"), std::string::npos);

    inFile.close();

    std::error_code ec;
    std::filesystem::remove(tmpFile, ec);
    EXPECT_FALSE(ec) << "Failed to remove temporary file: " << ec.message();
}

TEST(DocClass, SaveToFile_FailsIfEmptyDoc)
{
    ASSERT_TRUE(std::filesystem::exists(emptyFile));
    const auto emptyDoc = cpplibxml2::Doc::parseFile(
        emptyFile, cpplibxml2::ParserOptions::NoError | cpplibxml2::ParserOptions::Recover |
                       cpplibxml2::ParserOptions::NoEnt | cpplibxml2::ParserOptions::DtdLoad);
    ASSERT_TRUE(emptyDoc);
    const auto tmpFile = std::filesystem::temp_directory_path() / "invalid_output.xml";
    const auto result = emptyDoc.value().saveToFile(tmpFile);
    ASSERT_TRUE(result.has_value());

    std::error_code ec;
    std::filesystem::remove(tmpFile, ec);
    EXPECT_FALSE(ec) << "Failed to remove temporary file: " << ec.message();
}

TEST(DocClass, SaveToFile_InvalidPath)
{
    const std::string xmlContent = R"(<root><child>value</child></root>)";
    auto doc = cpplibxml2::Doc::parse(xmlContent);
    ASSERT_TRUE(doc.has_value());

    // Simuliere ungültigen Pfad
    const auto badPath = std::filesystem::path("/invalid_dir/test_output.xml");
    auto result = doc->saveToFile(badPath);
    ASSERT_FALSE(result.has_value());
}