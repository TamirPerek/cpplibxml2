#include "errorTypes.hpp"
#include <gtest/gtest.h>
#include <string>

using cpplibxml2::RuntimeError;

TEST(ErrorTest, DefaultConstructor) {
    const RuntimeError err;
    EXPECT_STREQ(err.what(), "");
}

TEST(ErrorTest, MessageConstructor) {
    const std::string msg = "Something went wrong";
    const RuntimeError err(msg);
    EXPECT_STREQ(err.what(), msg.c_str());
}

TEST(ErrorTest, ThrowAndCatch) {
    try {
        throw RuntimeError("Thrown error");
    } catch (const RuntimeError& e) {
        EXPECT_STREQ(e.what(), "Thrown error");
    }
}
