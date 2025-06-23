#include <gtest/gtest.h>

#include "helper.hpp"
#include <cpplibxml2.hpp>

#include <fstream>

static const std::filesystem::path exampleFile{"testData/example.xml"};
// static const std::filesystem::path emptyFile{"testData/empty.xml"};

TEST(NodeClass, Create) {
    ASSERT_TRUE(std::filesystem::exists(exampleFile));
    const auto DocRes = cpplibxml2::Doc::parseFile(exampleFile);
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    EXPECT_STREQ(Root.value().name().c_str(), "catalog");
}

TEST(NodeClass, MoveAssignmentOperator) {
    constexpr std::string_view xml = "<root><child>data</child></root>";
    constexpr std::string_view xml2 = "<root><child>data2</child></root>";
    const auto doc1 = cpplibxml2::Doc::parse(xml);
    const auto doc2 = cpplibxml2::Doc::parse(xml2);

    ASSERT_TRUE(doc1);
    ASSERT_TRUE(doc2);

    auto Root1 = doc1.value().root();
    auto Root2 = doc2.value().root();

    ASSERT_TRUE(Root1);
    ASSERT_TRUE(Root2);

    Root2.value() = std::move(Root1.value());
}

TEST(NodeClass, FindChild) {
    ASSERT_TRUE(std::filesystem::exists(exampleFile));
    const auto DocRes = cpplibxml2::Doc::parseFile(exampleFile);
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    EXPECT_STREQ(Root.value().name().c_str(), "catalog");
    const auto BookNode = Root.value().findChild("book");
    ASSERT_TRUE(BookNode);
    EXPECT_STREQ(BookNode.value().name().c_str(), "book");
    const auto PriceNode = BookNode.value().findChild("price");
    ASSERT_TRUE(PriceNode);
    EXPECT_STREQ(PriceNode.value().name().c_str(), "price");
    const auto NotANode = BookNode.value().findChild("Hello");
    EXPECT_FALSE(NotANode);
}

TEST(NodeClass, GetValue) {
    const auto DocRes = cpplibxml2::Doc::parse("<catalog><price>42.43</price></catalog>");
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    EXPECT_STREQ(Root.value().name().c_str(), "catalog");
    const auto PriceNode = Root.value().findChild("price");
    ASSERT_TRUE(PriceNode);
    EXPECT_STREQ(PriceNode.value().value().c_str(), "42.43");
    EXPECT_EQ(PriceNode.value().valueAsInt(), 42);
    EXPECT_EQ(PriceNode.value().valueAsLong(), 42l);
    EXPECT_EQ(PriceNode.value().valueAsLongLong(), 42ll);
    EXPECT_FLOAT_EQ(PriceNode.value().valueAsFloat(), 42.43f);
    EXPECT_DOUBLE_EQ(PriceNode.value().valueAsDouble(), 42.43);
}

TEST(NodeClass, GetValueNotANumber) {
    const auto DocRes = cpplibxml2::Doc::parse("<catalog><price>Hello</price></catalog>");
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    EXPECT_STREQ(Root.value().name().c_str(), "catalog");
    const auto PriceNode = Root.value().findChild("price");
    ASSERT_TRUE(PriceNode);
    EXPECT_STREQ(PriceNode.value().value().c_str(), "Hello");
    EXPECT_THROW(std::ignore = PriceNode.value().valueAsInt(), std::invalid_argument);
    EXPECT_THROW(std::ignore = PriceNode.value().valueAsFloat(), std::invalid_argument);
    EXPECT_THROW(std::ignore = PriceNode.value().valueAsDouble(), std::invalid_argument);
}

TEST(NodeClass, GetValueOfEmptyNode) {
    const auto DocRes = cpplibxml2::Doc::parse("<catalog></catalog>");
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    EXPECT_STREQ(Root.value().name().c_str(), "catalog");
    EXPECT_STREQ(Root.value().value().c_str(), "");
}
