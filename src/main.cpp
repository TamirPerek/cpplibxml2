#include "cpplibxml2.hpp"

#include <libxml/parser.h>
// #include <libxml/tree.h>

#include <memory>

namespace cpplibxml2
{
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

std::expected<Doc, Error> Doc::parseFile(const std::filesystem::path &path, ParserOptions options) noexcept
{
    // Initialize the library and check potential ABI mismatches
    LIBXML_TEST_VERSION

    if (!std::filesystem::exists(path))
        return std::unexpected{Error{"Document don't exist."}};

    auto doc = xmlDocPtr_t(xmlReadFile(path.string().c_str(), nullptr, static_cast<int>(options)));

    if (!doc)
    {
        return std::unexpected{Error{"Document not parsed successfully."}};
    }

    auto result = Doc{};
    result.impl->doc = std::move(doc);

    return result;
}

std::expected<Doc, Error> Doc::parse(std::string_view input, ParserOptions options) noexcept
{
    // Initialize the library and check potential ABI mismatches
    LIBXML_TEST_VERSION

    if (input.empty())
        return std::unexpected{Error{"Document is empty."}};

    auto doc = xmlDocPtr_t(
        xmlReadMemory(input.data(), static_cast<int>(input.size()), nullptr, nullptr, static_cast<int>(options)));

    if (!doc)
    {
        return std::unexpected{Error{"Document not parsed successfully."}};
    }

    auto result = Doc{};
    result.impl->doc = std::move(doc);

    return result;
}

struct Node::Impl
{
    xmlNodePtr node;
};

std::expected<Node, Error> Doc::root() const noexcept
{
    const auto root = xmlDocGetRootElement(this->impl->doc.get());
    if (!root)
        return std::unexpected{Error{"Document has no root node."}};

    return Node{root};
}

Node::Node() : impl(std::make_unique<Impl>())
{
}
Node::Node(xmlNodePtr in)  : impl(std::make_unique<Impl>())
{
    this->impl->node = in;
}

Node::Node(Node &&) noexcept = default;

Node::~Node() = default;

Node &Node::operator=(Node &&) noexcept = default;

std::string_view Node::name() const noexcept
{
    if (!this->impl->node)
        return "";
    return std::string_view{reinterpret_cast<const char *>(this->impl->node->name)};
}

std::expected<Node, Error> Node::findChild(std::string_view name) const noexcept
{
    if (!this->impl->node)
        return std::unexpected{Error{"Node not found."}};
    for (auto node = this->impl->node->children; node; node = node->next)
    {
        if (node->type != XML_ELEMENT_NODE)
            continue;

        if (std::string{reinterpret_cast<const char *>(node->name)} == name)
        {
            return Node{node};
        }
    }
    return std::unexpected{Error{"Node not found."}};
}

std::expected<Node, Error> Node::findChild(std::string_view name, std::string_view nsUri) const noexcept
{
    if (!this->impl->node)
        return std::unexpected{Error{"Node is null."}};

    for (auto node = this->impl->node->children; node; node = node->next)
    {
        if (node->type != XML_ELEMENT_NODE)
            continue;

        const std::string_view localName = reinterpret_cast<const char *>(node->name);
        const std::string_view href = node->ns && node->ns->href ? reinterpret_cast<const char *>(node->ns->href) : "";

        if (name == localName && nsUri == href)
        {
            return Node{node};
        }
    }

    return std::unexpected{Error{"Namespaced node not found."}};
}

std::expected<std::vector<Node>, Error> Node::getChildren() const noexcept
{
    if (!this->impl->node)
        return std::unexpected{Error{"Node is null."}};

    std::vector<Node> result;
    for (auto node = this->impl->node->children; node; node = node->next)
    {
        if (node->type != XML_ELEMENT_NODE)
            continue;

        result.emplace_back(Node{node});
    }

    return result;
}

using xmlChar_t = std::unique_ptr<xmlChar, decltype([](xmlChar *in) { xmlFree(in); })>;

std::string Node::value() const noexcept
{
    if (!this->impl->node)
        return "";
    const auto content = xmlChar_t{xmlNodeGetContent(this->impl->node)};
    if (!content)
        return "";

    std::string result(reinterpret_cast<const char *>(content.get()));
    return result;
}

float Node::valueAsFloat() const
{
    return std::stof(this->value());
}

double Node::valueAsDouble() const
{
    return std::stod(this->value());
}

int Node::valueAsInt(int base) const
{
    return std::stoi(this->value(), nullptr, base);
}

long Node::valueAsLong(int base) const
{
    return std::stol(this->value(), nullptr, base);
}

long long Node::valueAsLongLong(int base) const
{
    return std::stoll(this->value(), nullptr, base);
}
std::expected<std::pair<std::string_view, std::string_view>, Error> Node::findProperty(
    std::string_view name) const noexcept
{
    if (!this->impl->node)
        return std::unexpected{Error{"Node not found."}};
    for (auto node = this->impl->node->properties; node; node = node->next)
    {
        if (auto nodeName = std::string_view{reinterpret_cast<const char *>(node->name)}; nodeName == name)
        {
            return std::pair<std::string_view, std::string_view>{
                nodeName, node->children ? reinterpret_cast<const char *>(node->children->content) : ""};
        }
    }
    return std::unexpected{Error{"Property not found."}};
}

std::vector<std::pair<std::string_view, std::string_view>> Node::getProperties() const noexcept
{
    if (!this->impl->node)
        return {};

    std::vector<std::pair<std::string_view, std::string_view>> result;
    for (auto node = this->impl->node->properties; node; node = node->next)
    {
        result.emplace_back(std::string_view{reinterpret_cast<const char *>(node->name)}, node->children ? reinterpret_cast<const char *>(node->children->content) : "");
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

std::expected<Node, Error> Node::addChild(std::string_view name) const noexcept
{
    if (!this->impl->node)
        return std::unexpected{Error{"Node not found."}};

    const auto child = xmlNewNode(nullptr, reinterpret_cast<const unsigned char *>(name.data()));

    if (!child)
        return std::unexpected{Error{"Failed to create node."}};

    const auto newNode = xmlAddChild(this->impl->node, child);
    if (!newNode)
        return std::unexpected{Error{"Failed to add node."}};

    return Node{newNode};
}
} // namespace cpplibxml2
