#include "cpplibxml2.hpp"
#include <gtest/gtest.h>

using cpplibxml2::ParserOptions;

TEST(ParserOptionsTest, BitwiseOrOperator) {
    const ParserOptions opt = ParserOptions::NoEnt | ParserOptions::DtdLoad;
    EXPECT_TRUE((opt & ParserOptions::NoEnt) == ParserOptions::NoEnt);
    EXPECT_TRUE((opt & ParserOptions::DtdLoad) == ParserOptions::DtdLoad);
}

TEST(ParserOptionsTest, BitwiseAndOperator) {
    const ParserOptions opt = ParserOptions::NoEnt | ParserOptions::DtdLoad;
    EXPECT_EQ(opt & ParserOptions::NoEnt, ParserOptions::NoEnt);
    EXPECT_EQ(opt & ParserOptions::DtdLoad, ParserOptions::DtdLoad);
    EXPECT_EQ(opt & ParserOptions::Recover, static_cast<ParserOptions>(0));
}

TEST(ParserOptionsTest, BitwiseNotOperator) {
    const ParserOptions all = ParserOptions::NoEnt | ParserOptions::DtdLoad;
    const ParserOptions inv = ~all;
    EXPECT_EQ(inv & ParserOptions::Recover, ParserOptions::Recover);
    EXPECT_EQ(inv & ParserOptions::NoEnt, static_cast<ParserOptions>(0));
}

TEST(ParserOptionsTest, CompoundOrAssignmentOperator) {
    auto opt = ParserOptions::NoEnt;
    opt |= ParserOptions::DtdLoad;
    EXPECT_TRUE((opt & ParserOptions::DtdLoad) == ParserOptions::DtdLoad);
}

TEST(ParserOptionsTest, CompoundAndAssignmentOperator) {
    ParserOptions opt = ParserOptions::NoEnt | ParserOptions::DtdLoad;
    opt &= ParserOptions::NoEnt;
    EXPECT_EQ(opt, ParserOptions::NoEnt);
}
