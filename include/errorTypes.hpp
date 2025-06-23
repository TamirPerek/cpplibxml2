#pragma once
#include <stdexcept>

namespace cpplibxml2 {
    class Error final : public std::runtime_error {
        using std::runtime_error::runtime_error;
    public:
        Error() : std::runtime_error("") {}
    };
}
