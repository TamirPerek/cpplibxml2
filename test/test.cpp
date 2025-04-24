#include <gtest/gtest.h>
#include "cpplibxml2.hpp"

TEST(HelloWorldTest, BasicAssertions) {
    // Expect to be equal to 1
    EXPECT_EQ(1, 1);
}

TEST(LibXml2Test, TestFunction) {
    // Expect to be equal to 1
    auto res = cpplibxml2::test("testData/example.xml");
    ASSERT_EQ(res, 0);
}