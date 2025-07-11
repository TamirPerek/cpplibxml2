#include <gtest/gtest.h>

#include "helper.hpp"
#include <cpplibxml2.hpp>

class NodeNamespaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        xml = R"(<?xml version="1.0"?><root><child>data</child></root>)";
        doc = cpplibxml2::Doc::parse(xml).value();
        root = doc.root().value();
    }

    std::string_view xml{R"(<?xml version="1.0"?><root><child>data</child></root>)"};
    cpplibxml2::Doc doc{cpplibxml2::Doc::parse(xml).value()};
    cpplibxml2::Node root{doc.root().value()};
};

TEST_F(NodeNamespaceTest, AddNamespaceSuccess) {
    ASSERT_NO_THROW(root.addNamespace("test", "https://test.com"));
    auto [prefix, uri] = root.getNamespace();
    EXPECT_EQ(prefix, "test");
    EXPECT_EQ(uri, "https://test.com");
}

// TEST_F(NodeNamespaceTest, AddNamespaceToInvalidNode) {
//     auto doc = cpplibxml2::Doc::parse("", cpplibxml2::ParserOptions::Recover | cpplibxml2::ParserOptions::NoEnt | cpplibxml2::ParserOptions::DtdLoad);
//     ASSERT_TRUE(doc);
//     auto invalidNodeRes = doc.value().root();
//     ASSERT_TRUE(invalidNodeRes) << invalidNodeRes.error().what();
//     const auto invalidNode{std::move(invalidNodeRes.value())};
//     EXPECT_THROW({
//         try {
//             invalidNode.addNamespace("test", "https://test.com");
//         } catch (const cpplibxml2::Error& e) {
//             EXPECT_STREQ(e.what(), "Node not found.");
//             throw;
//         }
//     }, cpplibxml2::Error);
// }

TEST_F(NodeNamespaceTest, AddEmptyNamespace) {
    ASSERT_NO_THROW(root.addNamespace("", "https://test.com"));
    auto [prefix, uri] = root.getNamespace();
    EXPECT_TRUE(prefix.empty());
    EXPECT_EQ(uri, "https://test.com");
}

TEST_F(NodeNamespaceTest, AddNamespaceAndRemove) {
    ASSERT_NO_THROW(root.addNamespace("test", "https://test.com"));
    ASSERT_NO_THROW(root.removeNamespace());
    auto [prefix, uri] = root.getNamespace();
    EXPECT_TRUE(prefix.empty());
    EXPECT_TRUE(uri.empty());
}

TEST_F(NodeNamespaceTest, AddMultipleNamespaces) {
    ASSERT_NO_THROW(root.addNamespace("test1", "https://test1.com"));
    auto [prefix1, uri1] = root.getNamespace();
    EXPECT_EQ(prefix1, "test1");
    EXPECT_EQ(uri1, "https://test1.com");

    ASSERT_NO_THROW(root.addNamespace("test2", "https://test2.com"));
    auto [prefix2, uri2] = root.getNamespace();
    EXPECT_EQ(prefix2, "test2");
    EXPECT_EQ(uri2, "https://test2.com");
}