#pragma once
#include <stdexcept>

namespace cpplibxml2 {
class RuntimeError final : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;

    RuntimeError() : std::runtime_error("")
    {
    }
};

class InvalidArgument final : public std::invalid_argument
{
  public:
    using std::invalid_argument::invalid_argument;
    explicit InvalidArgument(const invalid_argument &arg) : std::invalid_argument(arg.what())
    {
    }
};
} // namespace cpplibxml2
