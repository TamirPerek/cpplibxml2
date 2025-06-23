#include "cpplibxml2.hpp"

#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include <memory>

namespace cpplibxml2 {
    struct xmlDocDeleter {
        void operator()(xmlDoc *doc) const {
            if (doc) {
                xmlFreeDoc(doc);
            }
        }
    };

    using xmlDocPtr_t = std::unique_ptr<xmlDoc, xmlDocDeleter>;

    struct Doc::Impl {
        xmlDocPtr_t doc;
    };

    Doc::Doc()
        : impl(std::make_unique<Impl>()) {
    }

    Doc::Doc(Doc &&) noexcept = default;

    Doc::~Doc() = default;

    Doc &Doc::operator=(Doc &&) noexcept = default;

    std::expected<Doc, Error> Doc::parseFile(const std::filesystem::path &path, ParserOptions options) noexcept {
        // Initialize the library and check potential ABI mismatches
        LIBXML_TEST_VERSION

        if (!std::filesystem::exists(path))
            return std::unexpected{Error{"Document don't exist."}};

        auto doc = xmlDocPtr_t(xmlReadFile(path.string().c_str(), nullptr, static_cast<int>(options)));

        if (!doc) {
            return std::unexpected{ Error{"Document not parsed successfully."}};
        }

        auto result = Doc{};
        result.impl->doc = std::move(doc);

        return result;
    }

    std::expected<Doc, Error> Doc::parse(std::string_view input, ParserOptions options) noexcept {
        // Initialize the library and check potential ABI mismatches
        LIBXML_TEST_VERSION

        if (input.empty())
            return std::unexpected{Error{"Document is empty."}};

        auto doc = xmlDocPtr_t(xmlReadMemory(input.data(), static_cast<int>(input.size()), nullptr, nullptr, static_cast<int>(options)));

        if (!doc) {
            return std::unexpected{Error{"Document not parsed successfully."}};
        }

        auto result = Doc{};
        result.impl->doc = std::move(doc);

        return result;
    }

    struct Node::Impl {
        xmlNodePtr node;
    };

    std::expected<Node, Error> Doc::root() const noexcept {
        const auto root = xmlDocGetRootElement(this->impl->doc.get());
        if (!root)
            return std::unexpected{Error{"Document has no root node."}};

        auto result = Node{};
        result.impl->node = root;
        return result;
    }

    Node::Node()
        : impl(std::make_unique<Impl>())
    {}

    Node::Node(Node &&) noexcept = default;

    Node::~Node()  = default;

    Node & Node::operator=(Node &&) noexcept  = default;

    std::string Node::name() const noexcept {
        if (!this->impl->node)
            return "";
        return std::string{reinterpret_cast<const char *>(this->impl->node->name)};
    }

    std::expected<Node, Error> Node::findChild(std::string_view name) const noexcept {
        if (!this->impl->node)
            return std::unexpected{Error{"Node not found."}};
        for (auto node = this->impl->node->children; node; node = node->next) {
            if (node->type != XML_ELEMENT_NODE)
                continue;

            if (std::string{reinterpret_cast<const char *>(node->name)} == name) {
                Node res;
                res.impl->node = node;
                return res;
            }
        }
        return std::unexpected{Error{"Node not found."}};
    }

    using xmlChar_t = std::unique_ptr<xmlChar, decltype([](xmlChar *in){xmlFree(in);})>;

    std::string Node::value() const noexcept {
        if (!this->impl->node)
            return "";
        const auto content = xmlChar_t{xmlNodeGetContent(this->impl->node)};
        if (!content)
            return "";

        std::string result(reinterpret_cast<const char *>(content.get()));
        return result;
    }

    float Node::valueAsFloat() const {
        return std::stof(this->value());
    }

    double Node::valueAsDouble() const {
        return std::stod(this->value());
    }

    int Node::valueAsInt(int base) const {
        return std::stoi(this->value(), nullptr, base);
    }

    long Node::valueAsLong(int base) const {
        return std::stol(this->value(), nullptr, base);
    }

    long long Node::valueAsLongLong(int base) const {
        return std::stoll(this->value(), nullptr, base);
    }
}
