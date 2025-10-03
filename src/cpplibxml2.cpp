#include "cpplibxml2.hpp"

#include "helper.hpp"

#include <functional>
#include <libxml/parser.h>

#include <memory>

namespace cpplibxml2
{

struct Doc::Impl
{
    xmlDocPtr_t doc;
};

Doc::Doc() : impl(std::make_unique<Impl>())
{
}

Doc::Doc(Doc &&) noexcept = default;

Doc::~Doc() = default;

Doc &Doc::operator=(Doc &&) noexcept = default;

std::expected<Doc, RuntimeError> Doc::parseFile(const std::filesystem::path &path, ParserOptions options) noexcept
{
    // Initialize the library and check potential ABI mismatches
    LIBXML_TEST_VERSION

    if (!std::filesystem::exists(path))
        return std::unexpected{RuntimeError{"Document don't exist."}};

    auto doc = xmlDocPtr_t(xmlReadFile(path.string().c_str(), nullptr, static_cast<int>(options)));

    if (!doc)
    {
        return std::unexpected{RuntimeError{"Document not parsed successfully."}};
    }

    auto result = Doc{};
    result.impl->doc = std::move(doc);

    return result;
}

std::expected<Doc, RuntimeError> Doc::parse(std::string_view input, ParserOptions options) noexcept
{
    // Initialize the library and check potential ABI mismatches
    LIBXML_TEST_VERSION

    if (input.empty())
        return std::unexpected{RuntimeError{"Document is empty."}};

    auto doc = xmlDocPtr_t(
        xmlReadMemory(input.data(), static_cast<int>(input.size()), nullptr, nullptr, static_cast<int>(options)));

    if (!doc)
    {
        return std::unexpected{RuntimeError{"Document not parsed successfully."}};
    }

    auto result = Doc{};
    result.impl->doc = std::move(doc);

    return result;
}

struct Node::Impl
{
    xmlNodePtr node;
};

std::expected<Node, RuntimeError> Doc::root() const noexcept
{
    const auto root = xmlDocGetRootElement(this->impl->doc.get());
    if (!root)
        return std::unexpected{RuntimeError{"Document has no root node."}};

    auto rootNode = Node{};
    rootNode.impl->node = root;
    return rootNode;
}
std::expected<std::string, RuntimeError> Doc::dump(bool addWhiteSpaces, Format format) const noexcept
{
    if (!this->impl->doc)
        return std::unexpected{RuntimeError{"Document is null."}};

    xmlChar *buffer = nullptr;
    int size = -1;
    xmlDocDumpFormatMemoryEnc(this->impl->doc.get(), &buffer, &size, to_string(format).c_str(), addWhiteSpaces ? 1 : 0);
    if (!buffer)
        return std::unexpected{RuntimeError{"Failed to dump document."}};

    std::string result(reinterpret_cast<const char *>(buffer), static_cast<std::string::size_type>(size));
    return result;
}

std::expected<void, RuntimeError> Doc::saveToFile(const std::filesystem::path &path, bool addWhiteSpaces,
                                                  Format format) const noexcept
{
    if (!this->impl->doc)
        return std::unexpected{RuntimeError{"Document is null."}};

    const std::string encoding = to_string(format);
    const int formatFlag = addWhiteSpaces ? 1 : 0;

    const int rc = xmlSaveFormatFileEnc(path.string().c_str(), this->impl->doc.get(), encoding.c_str(), formatFlag);

    if (rc == -1)
        return std::unexpected{RuntimeError{"Failed to write XML document to file."}};

    return {};
}

Node::Node() : impl(std::make_unique<Impl>())
{
}

Node::Node(Node &&) noexcept = default;

Node::~Node() = default;

Node &Node::operator=(Node &&) noexcept = default;

std::expected<std::string_view, RuntimeError> Node::name() const noexcept
{
    if (!this->impl->node)
        return std::unexpected{RuntimeError{"Node is null."}};
    return std::string_view{reinterpret_cast<const char *>(this->impl->node->name)};
}

std::expected<Node, RuntimeError> Node::findChild(std::string_view name) const noexcept
{
    if (!this->impl->node)
        return std::unexpected{RuntimeError{"Node not found."}};
    for (auto node = this->impl->node->children; node; node = node->next)
    {
        if (node->type != XML_ELEMENT_NODE)
            continue;

        if (std::string{reinterpret_cast<const char *>(node->name)} == name)
        {
            auto rootNode = Node{};
            rootNode.impl->node = node;
            return rootNode;
        }
    }
    return std::unexpected{RuntimeError{"Node not found."}};
}

std::expected<Node, RuntimeError> Node::findChild(std::string_view name, std::string_view nsUri) const noexcept
{
    if (!this->impl->node)
        return std::unexpected{RuntimeError{"Node is null."}};

    for (auto node = this->impl->node->children; node; node = node->next)
    {
        if (node->type != XML_ELEMENT_NODE)
            continue;

        const std::string_view localName = reinterpret_cast<const char *>(node->name);
        const std::string_view href = node->ns && node->ns->href ? reinterpret_cast<const char *>(node->ns->href) : "";

        if (name == localName && nsUri == href)
        {
            auto rootNode = Node{};
            rootNode.impl->node = node;
            return rootNode;
        }
    }

    return std::unexpected{RuntimeError{"Namespaced node not found."}};
}

std::expected<std::vector<Node>, RuntimeError> Node::getChildren() const noexcept
{
    if (!this->impl->node)
        return std::unexpected{RuntimeError{"Node is null."}};

    std::vector<Node> result;
    for (auto node = this->impl->node->children; node; node = node->next)
    {
        if (node->type != XML_ELEMENT_NODE)
            continue;

        auto rootNode = Node{};
        rootNode.impl->node = node;
        result.emplace_back(std::move(rootNode));
    }

    return result;
}

using xmlChar_t = std::unique_ptr<xmlChar, decltype([](xmlChar *in) { xmlFree(in); })>;

std::expected<std::string, RuntimeError> Node::value() const noexcept
{
    if (!this->impl->node)
        return std::unexpected{RuntimeError{"Node is null."}};
    const auto content = xmlChar_t{xmlNodeGetContent(this->impl->node)};
    if (!content)
        return std::unexpected{RuntimeError{"Failed to get node content."}};

    std::string result(reinterpret_cast<const char *>(content.get()));
    return result;
}

std::expected<float, InvalidArgument> Node::valueAsFloat() const
{
    auto lambda = [](const std::string &in) { return std::stof(in); };
    return this->value()
        .transform_error([](auto &&in) { return InvalidArgument{std::string{in.what()}}; })
        .and_then([lambda](std::string &&in) { return to_value<std::string, float>(lambda, std::move(in)); });
}

std::expected<double, InvalidArgument> Node::valueAsDouble() const
{
    auto lambda = [](const std::string &in) { return std::stod(in); };
    return this->value()
        .transform_error([](auto &&in) { return InvalidArgument{std::string{in.what()}}; })
        .and_then([lambda](std::string &&in) { return to_value<std::string, double>(lambda, std::move(in)); });
}

std::expected<int, InvalidArgument> Node::valueAsInt(int base) const
{
    auto lambda = [base](const std::string &in) { return std::stoi(in, nullptr, base); };
    return this->value()
        .transform_error([](auto &&in) { return InvalidArgument{std::string{in.what()}}; })
        .and_then([lambda](std::string &&in) { return to_value<std::string, int>(lambda, std::move(in)); });
}

std::expected<long, InvalidArgument> Node::valueAsLong(int base) const
{
    auto lambda = [base](const std::string &in) { return std::stol(in, nullptr, base); };
    return this->value()
        .transform_error([](auto &&in) { return InvalidArgument{std::string{in.what()}}; })
        .and_then([lambda](std::string &&in) { return to_value<std::string, long>(lambda, std::move(in)); });
}

std::expected<long long, InvalidArgument> Node::valueAsLongLong(int base) const
{
    auto lambda = [base](const std::string &in) { return std::stoll(in, nullptr, base); };
    return this->value()
        .transform_error([](auto &&in) { return InvalidArgument{std::string{in.what()}}; })
        .and_then([lambda](std::string &&in) { return to_value<std::string, long long>(lambda, std::move(in)); });
}
std::expected<std::pair<std::string_view, std::string_view>, RuntimeError> Node::findProperty(
    const std::string_view name) const noexcept
{
    if (!this->impl->node)
        return std::unexpected{RuntimeError{"Node not found."}};
    for (auto node = this->impl->node->properties; node; node = node->next)
    {
        if (auto nodeName = std::string_view{reinterpret_cast<const char *>(node->name)}; nodeName == name)
        {
            return std::pair<std::string_view, std::string_view>{
                nodeName, node->children ? reinterpret_cast<const char *>(node->children->content) : ""};
        }
    }
    return std::unexpected{RuntimeError{"Property not found."}};
}

std::vector<std::pair<std::string_view, std::string_view>> Node::getProperties() const noexcept
{
    if (!this->impl->node)
        return {};

    std::vector<std::pair<std::string_view, std::string_view>> result;
    for (auto node = this->impl->node->properties; node; node = node->next)
    {
        result.emplace_back(std::string_view{reinterpret_cast<const char *>(node->name)},
                            node->children ? reinterpret_cast<const char *>(node->children->content) : "");
    }

    return result;
}

std::pair<std::string_view, std::string_view> Node::getNamespace() const noexcept
{
    if (!this->impl->node)
        return {};
    if (this->impl->node->ns)
    {
        return {std::string_view{reinterpret_cast<const char *>(this->impl->node->ns->prefix)},
                std::string_view{reinterpret_cast<const char *>(this->impl->node->ns->href)}};
    }

    return {};
}

std::expected<Node, RuntimeError> Node::addChild(std::string_view name) const noexcept
{
    if (!this->impl->node)
        return std::unexpected{RuntimeError{"Node not found."}};

    const auto child = xmlNewNode(nullptr, reinterpret_cast<const unsigned char *>(name.data()));

    if (!child)
        return std::unexpected{RuntimeError{"Failed to create node."}};

    const auto newNode = xmlAddChild(this->impl->node, child);
    if (!newNode)
        return std::unexpected{RuntimeError{"Failed to add node."}};

    auto rootNode = Node{};
    rootNode.impl->node = newNode;
    return rootNode;
}

void Node::addValue(const std::string_view value) const
{
    if (!this->impl->node)
        throw RuntimeError{"Node not found."};

    const auto res = xmlNodeSetContent(this->impl->node, reinterpret_cast<const unsigned char *>(value.data()));
    if (res == 1)
        throw RuntimeError{"Failed to add value."};
    if (res == -1)
        throw RuntimeError{"Memory allocation to add this value failed"};
}

void Node::addNamespace(const std::string_view prefix, const std::string_view uri) const
{
    if (!this->impl->node)
        throw RuntimeError{"Node not found."};
    const auto ns = xmlNewNs(this->impl->node, reinterpret_cast<const unsigned char *>(uri.data()),
                             reinterpret_cast<const unsigned char *>(prefix.data()));
    if (!ns)
        throw RuntimeError{"Failed to add namespace."};
    xmlSetNs(this->impl->node, ns);
}

void Node::removeNamespace() const
{
    if (!this->impl->node)
        throw RuntimeError{"Node not found."};
    xmlSetNs(this->impl->node, nullptr);
}
} // namespace cpplibxml2
