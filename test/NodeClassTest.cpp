#include <gtest/gtest.h>

#include "helper.hpp"
#include <cpplibxml2.hpp>

#include <fstream>

static const std::filesystem::path exampleFile{"testData/example.xml"};
static const std::filesystem::path nsExampleFile{"testData/nsExample.xml"};
// static const std::filesystem::path emptyFile{"testData/empty.xml"};

TEST(NodeClass, Create)
{
    ASSERT_TRUE(std::filesystem::exists(exampleFile));
    const auto DocRes = cpplibxml2::Doc::parseFile(exampleFile);
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    ASSERT_TRUE(Root.value().name());
    EXPECT_STREQ(Root.value().name().value().data(), "catalog");
}

TEST(NodeClass, MoveAssignmentOperator)
{
    constexpr std::string_view xml = R"(<?xml version="1.0"?><root><child>data</child></root>)";
    constexpr std::string_view xml2 = R"(<?xml version="1.0"?><root><child>data2</child></root>)";
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

TEST(NodeClass, FindChild)
{
    ASSERT_TRUE(std::filesystem::exists(exampleFile));
    const auto DocRes = cpplibxml2::Doc::parseFile(exampleFile);
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    ASSERT_TRUE(Root.value().name());
    EXPECT_STREQ(Root.value().name().value().data(), "catalog");
    const auto BookNode = Root.value().findChild("book");
    ASSERT_TRUE(BookNode);
    ASSERT_TRUE(BookNode.value().name());
    EXPECT_STREQ(BookNode.value().name().value().data(), "book");
    const auto PriceNode = BookNode.value().findChild("price");
    ASSERT_TRUE(PriceNode);
    ASSERT_TRUE(PriceNode.value().name());
    EXPECT_STREQ(PriceNode.value().name().value().data(), "price");
    const auto NotANode = BookNode.value().findChild("Hello");
    EXPECT_FALSE(NotANode);
}

TEST(NodeClass, GetChildren)
{
    ASSERT_TRUE(std::filesystem::exists(exampleFile));
    const auto DocRes = cpplibxml2::Doc::parseFile(exampleFile);
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    ASSERT_TRUE(Root.value().name());
    EXPECT_STREQ(Root.value().name().value().data(), "catalog");
    const auto children = Root.value().getChildren();
    ASSERT_TRUE(children);
    EXPECT_EQ(children.value().size(), 12);
    EXPECT_TRUE(std::ranges::all_of(children.value(), [](const auto &child) { return child.name() == "book"; }));
}

TEST(NodeClass, GetValue)
{
    const auto DocRes = cpplibxml2::Doc::parse(R"(<?xml version="1.0"?><catalog><price>42.43</price></catalog>)");
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    ASSERT_TRUE(Root.value().name());
    EXPECT_STREQ(Root.value().name().value().data(), "catalog");
    const auto PriceNode = Root.value().findChild("price");
    ASSERT_TRUE(PriceNode);
    ASSERT_TRUE(PriceNode.value().name());
    EXPECT_STREQ(PriceNode.value().value().value().c_str(), "42.43");
    EXPECT_EQ(PriceNode.value().valueAsInt(), 42);
    EXPECT_EQ(PriceNode.value().valueAsLong(), 42l);
    EXPECT_EQ(PriceNode.value().valueAsLongLong(), 42ll);
    ASSERT_TRUE(PriceNode.value().valueAsFloat());
    EXPECT_FLOAT_EQ(PriceNode.value().valueAsFloat().value(), 42.43f);
    ASSERT_TRUE(PriceNode.value().valueAsDouble());
    EXPECT_DOUBLE_EQ(PriceNode.value().valueAsDouble().value(), 42.43);
}

TEST(NodeClass, GetValueNotANumber)
{
    const auto DocRes = cpplibxml2::Doc::parse(R"(<?xml version="1.0"?><catalog><price>Hello</price></catalog>)");
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    ASSERT_TRUE(Root.value().name());
    EXPECT_STREQ(Root.value().name().value().data(), "catalog");
    const auto PriceNode = Root.value().findChild("price");
    ASSERT_TRUE(PriceNode);
    ASSERT_TRUE(PriceNode.value().value());
    EXPECT_STREQ(PriceNode.value().value().value().c_str(), "Hello");

    // Unable to convert strings to numbers
    EXPECT_FALSE(PriceNode.value().valueAsInt());
    EXPECT_FALSE(PriceNode.value().valueAsLong());
    EXPECT_FALSE(PriceNode.value().valueAsLongLong());
    EXPECT_FALSE(PriceNode.value().valueAsFloat());
    EXPECT_FALSE(PriceNode.value().valueAsDouble());

    // Convert value to number
    PriceNode.value().addValue("42.4");
    ASSERT_TRUE(PriceNode.value().valueAsInt());
    EXPECT_EQ(PriceNode.value().valueAsInt().value(), 42);
    ASSERT_TRUE(PriceNode.value().valueAsLong());
    EXPECT_EQ(PriceNode.value().valueAsLong().value(), 42l);
    ASSERT_TRUE(PriceNode.value().valueAsLongLong());
    EXPECT_EQ(PriceNode.value().valueAsLongLong().value(), 42ll);
    ASSERT_TRUE(PriceNode.value().valueAsFloat());
    EXPECT_EQ(PriceNode.value().valueAsFloat().value(), 42.4f);
    ASSERT_TRUE(PriceNode.value().valueAsDouble());
    EXPECT_EQ(PriceNode.value().valueAsDouble().value(), 42.4);
}

TEST(NodeClass, GetValueOfEmptyNode)
{
    const auto DocRes = cpplibxml2::Doc::parse(R"(<?xml version="1.0"?><catalog></catalog>)");
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    ASSERT_TRUE(Root.value().name());
    EXPECT_STREQ(Root.value().name().value().data(), "catalog");
    ASSERT_TRUE(Root.value().value());
    EXPECT_STREQ(Root.value().value().value().c_str(), "");
}

TEST(NodeClass, GetProperty)
{
    const auto DocRes =
        cpplibxml2::Doc::parse(R"(<?xml version="1.0"?><catalog id="Hello" class="World" empty=""></catalog>)");
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    auto id = Root.value().findProperty("id");
    ASSERT_TRUE(id);
    EXPECT_STREQ(id.value().first.data(), "id");
    EXPECT_STREQ(id.value().second.data(), "Hello");
    auto classRes = Root.value().findProperty("class");
    ASSERT_TRUE(classRes);
    EXPECT_STREQ(classRes.value().first.data(), "class");
    EXPECT_STREQ(classRes.value().second.data(), "World");
    auto notAProperty = Root.value().findProperty("Hello");
    EXPECT_FALSE(notAProperty);
    auto emptyProperty = Root.value().findProperty("empty");
    ASSERT_TRUE(emptyProperty);
    EXPECT_STREQ(emptyProperty.value().first.data(), "empty");
    EXPECT_STREQ(emptyProperty.value().second.data(), "");
}

TEST(NodeClass, GetProperties)
{
    const auto DocRes = cpplibxml2::Doc::parse(R"(<?xml version="1.0"?><catalog id="Hello" class="World"></catalog>)");
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    const auto properties = Root.value().getProperties();
    EXPECT_TRUE(
        std::ranges::any_of(properties, [](const auto &in) { return in.first == "id" && in.second == "Hello"; }));
    EXPECT_TRUE(
        std::ranges::any_of(properties, [](const auto &in) { return in.first == "class" && in.second == "World"; }));
}

TEST(NodeClass, GetNamespace)
{
    ASSERT_TRUE(std::filesystem::exists(nsExampleFile));
    const auto DocRes = cpplibxml2::Doc::parseFile(nsExampleFile);
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    ASSERT_TRUE(Root.value().name());
    EXPECT_STREQ(Root.value().name().value().data(), "ConnectorServices");
    EXPECT_STREQ(Root.value().getNamespace().first.data(), "ns2");
    EXPECT_STREQ(Root.value().getNamespace().second.data(), "http://ws.gematik.de/conn/ServiceDirectory/v3.1");
}

TEST(NodeClass, GetNodeByNamespace)
{
    ASSERT_TRUE(std::filesystem::exists(nsExampleFile));
    const auto DocRes = cpplibxml2::Doc::parseFile(nsExampleFile);
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    const auto nsNode =
        Root.value().findChild("ServiceInformation", "http://ws.gematik.de/conn/ServiceInformation/v2.0");
    ASSERT_TRUE(nsNode);
    ASSERT_TRUE(nsNode.value().name());
    EXPECT_STREQ(nsNode.value().name().value().data(), "ServiceInformation");
    const auto [ns, uri] = nsNode.value().getNamespace();
    EXPECT_STREQ(ns.data(), "ns3");
    EXPECT_STREQ(uri.data(), "http://ws.gematik.de/conn/ServiceInformation/v2.0");
}

TEST(NodeClass, AddNode)
{
    const auto DocRes = cpplibxml2::Doc::parse(R"(<?xml version="1.0"?><catalog></catalog>)");
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    ASSERT_TRUE(Root.value().name());
    EXPECT_STREQ(Root.value().name().value().data(), "catalog");
    const auto newNode = Root.value().addChild("price");
    ASSERT_TRUE(newNode);
    ASSERT_TRUE(newNode.value().name());
    EXPECT_STREQ(newNode.value().name().value().data(), "price");
    const auto priceNode = Root.value().findChild("price");
    ASSERT_TRUE(priceNode);
    ASSERT_TRUE(priceNode.value().name());
    EXPECT_STREQ(priceNode.value().name().value().data(), "price");
}

TEST(NodeClass, AddValueToNode)
{
    const auto DocRes = cpplibxml2::Doc::parse(R"(<?xml version="1.0"?><catalog></catalog>)");
    ASSERT_TRUE(DocRes);
    const auto Root = DocRes.value().root();
    ASSERT_TRUE(Root);
    ASSERT_TRUE(Root.value().name());
    EXPECT_STREQ(Root.value().name().value().data(), "catalog");
    const auto newNode = Root.value().addChild("price");
    ASSERT_TRUE(newNode);
    ASSERT_TRUE(newNode.value().name());
    EXPECT_STREQ(newNode.value().name().value().data(), "price");
    newNode.value().addValue("Hello World!");
    ASSERT_TRUE(newNode.value().value());
    EXPECT_STREQ(newNode.value().value().value().c_str(), "Hello World!");
}
