#pragma once

#include "errorTypes.hpp"

#include <libxml/parser.h>

#include <expected>
#include <functional>

namespace cpplibxml2
{
template <typename InputType, typename OutputType>
std::expected<OutputType, InvalidArgument> to_value(std::function<OutputType(InputType)> &&func, InputType &&in)
{
    try
    {
        return func(std::forward<InputType>(in));
    }
    catch (const std::invalid_argument &e)
    {
        return std::unexpected{InvalidArgument{e}};
    }
    catch (const std::out_of_range &e)
    {
        return std::unexpected{InvalidArgument{e.what()}};
    }
}

struct xmlDocDeleter
{
    void operator()(xmlDoc *doc) const
    {
        if (doc)
        {
            xmlFreeDoc(doc);
        }
    }
};

using xmlDocPtr_t = std::unique_ptr<xmlDoc, xmlDocDeleter>;
} // namespace cpplibxml2
