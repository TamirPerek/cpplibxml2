#include "errorTypes.hpp"
#include <gtest/gtest.h>
#include <string>

using cpplibxml2::Error;

TEST(ErrorTest, DefaultConstructor) {
    const Error err;
    EXPECT_STREQ(err.what(), "");
}

TEST(ErrorTest, MessageConstructor) {
    const std::string msg = "Something went wrong";
    const Error err(msg);
    EXPECT_STREQ(err.what(), msg.c_str());
}

TEST(ErrorTest, ThrowAndCatch) {
    try {
        throw Error("Thrown error");
    } catch (const Error& e) {
        EXPECT_STREQ(e.what(), "Thrown error");
    }
}
