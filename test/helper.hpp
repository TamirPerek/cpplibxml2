#pragma once

#include <string>
#include <random>
#include <algorithm>

inline std::string generateRandomString(const std::size_t length) {
    static constexpr char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static constexpr std::size_t max_index = sizeof(charset) - 2;

    thread_local std::mt19937 rng{std::random_device{}()};
    thread_local std::uniform_int_distribution<std::size_t> dist(0, max_index);

    std::string result;
    result.reserve(length);
    std::generate_n(std::back_inserter(result), length, [] {
        return charset[dist(rng)];
    });

    return result;
}