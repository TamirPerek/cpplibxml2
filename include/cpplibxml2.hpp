#pragma once

#include "errorTypes.hpp"

#include <expected>
#include <filesystem>
#include <memory>
#include <type_traits>
#include <vector>

// Forward declaration
typedef struct _xmlNode xmlNode;
typedef xmlNode *xmlNodePtr;

namespace cpplibxml2
{
enum class ParserOptions : int
{
    Recover = 1 << 0,         /* recover on errors */
    NoEnt = 1 << 1,           /* substitute entities */
    DtdLoad = 1 << 2,         /* load the external subset */
    DtdAttr = 1 << 3,         /* default DTD attributes */
    DtdValid = 1 << 4,        /* validate with the DTD */
    NoError = 1 << 5,         /* suppress error reports */
    NoWarning = 1 << 6,       /* suppress warning reports */
    Pedantic = 1 << 7,        /* pedantic error reporting */
    NoBlanks = 1 << 8,        /* remove blank nodes */
    XML_PARSE_SAX1 = 1 << 9,  /* use the SAX1 interface internally */
    XInclude = 1 << 10,       /* Implement XInclude substitution  */
    NoNet = 1 << 11,          /* Forbid network access */
    NoDict = 1 << 12,         /* Do not reuse the context dictionary */
    NsClean = 1 << 13,        /* remove redundant namespaces declarations */
    NoCData = 1 << 14,        /* merge CDATA as text nodes */
    NoXIncludeNode = 1 << 15, /* do not generate XINCLUDE START/END nodes */
    Compact = 1 << 16,        /* compact small text nodes; no modification of
                                           the tree allowed afterwards (will possibly
                                           crash if you try to modify the tree) */
    Old10 = 1 << 17,          /* parse using XML-1.0 before update 5 */
    NoBaseFix = 1 << 18,      /* do not fixup XINCLUDE xml:base uris */
    Huge = 1 << 19,           /* relax any hardcoded limit from the parser */
    OldSax = 1 << 20,         /* parse using SAX2 interface before 2.7.0 */
    IgnoreEnc = 1 << 21,      /* ignore internal document encoding hint */
    BigLines = 1 << 22,       /* Store big lines numbers in text PSVI field since 2.13.0 */
    NoXXE = 1 << 23,          /* disable loading of external content since 2.14.0 */
    Unzip = 1 << 24,          /* allow compressed content */
    NoSysCatalog = 1 << 25,   /* disable global system catalog */
    CatalogAPI = 1 << 26      /* allow catalog PIs */
};

inline ParserOptions operator|(ParserOptions lhs, ParserOptions rhs)
{
    using T = std::underlying_type_t<ParserOptions>;
    return static_cast<ParserOptions>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline ParserOptions operator&(ParserOptions lhs, ParserOptions rhs)
{
    using T = std::underlying_type_t<ParserOptions>;
    return static_cast<ParserOptions>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline ParserOptions operator~(ParserOptions opt)
{
    using T = std::underlying_type_t<ParserOptions>;
    return static_cast<ParserOptions>(~static_cast<T>(opt));
}

inline ParserOptions &operator|=(ParserOptions &lhs, ParserOptions rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

inline ParserOptions &operator&=(ParserOptions &lhs, ParserOptions rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

class Node;

enum class Format
{
    UTF_8,
    UTF_16,
    ISO_8859_1,
    ASCII
};

[[nodiscard]] std::string to_string(Format format);
inline std::string to_string(Format format)
{
    switch (format)
    {
    case Format::UTF_16:
        return "UTF-16";
    case Format::ISO_8859_1:
        return "ISO-8859-1";
    case Format::ASCII:
        return "ASCII";
    case Format::UTF_8:
        [[fallthrough]];
    default:
        return "UTF-8";
    }
}

class Doc
{
    struct Impl;
    std::unique_ptr<Impl> impl;

    Doc();

  public:
    Doc(const Doc &) = delete;

    Doc(Doc &&) noexcept;

    ~Doc();

    Doc &operator=(const Doc &) = delete;

    Doc &operator=(Doc &&) noexcept;

    [[nodiscard]] static std::expected<Doc, Error> parseFile(const std::filesystem::path &,
                                                             ParserOptions options = ParserOptions::NoEnt |
                                                                                     ParserOptions::DtdLoad) noexcept;

    [[nodiscard]] static std::expected<Doc, Error> parse(std::string_view,
                                                         ParserOptions = ParserOptions::NoEnt |
                                                                         ParserOptions::DtdLoad) noexcept;

    [[nodiscard]] std::expected<Node, Error> root() const noexcept;

    [[nodiscard]] std::expected<std::string, Error> dump(bool addWhiteSpaces = false,
                                                         Format format = Format::UTF_8) const noexcept;
};

class Node
{
    Node();
    explicit Node(xmlNodePtr);

    struct Impl;
    std::unique_ptr<Impl> impl;

    friend class Doc;

  public:
    Node(const Node &) = delete;

    Node(Node &&) noexcept;

    ~Node();

    Node &operator=(const Node &) = delete;

    Node &operator=(Node &&) noexcept;

    [[nodiscard]] std::string_view name() const noexcept;

    [[nodiscard]] std::expected<Node, Error> findChild(std::string_view name) const noexcept;

    [[nodiscard]] std::expected<Node, Error> findChild(std::string_view name, std::string_view nsUri) const noexcept;

    [[nodiscard]] std::expected<std::vector<Node>, Error> getChildren() const noexcept;

    [[nodiscard]] std::string value() const noexcept;

    /**
     *
     * @return Value as float
     * @throws std::invalid_argument if conversion fails
     */
    [[nodiscard]] float valueAsFloat() const;
    /**
     *
     * @return Value as double
     * @throws std::invalid_argument if conversion fails
     */
    [[nodiscard]] double valueAsDouble() const;
    /**
     *
     * @return Value as int
     * @throws std::invalid_argument if conversion fails
     */
    [[nodiscard]] int valueAsInt(int base = 10) const;
    /**
     *
     * @return Value as long
     * @throws std::invalid_argument if conversion fails
     */
    [[nodiscard]] long valueAsLong(int base = 10) const;
    /**
     *
     * @return Value as long long
     * @throws std::invalid_argument if conversion fails
     */
    [[nodiscard]] long long valueAsLongLong(int base = 10) const;

    [[nodiscard]] std::expected<std::pair<std::string_view, std::string_view>, Error> findProperty(
        std::string_view name) const noexcept;

    [[nodiscard]] std::vector<std::pair<std::string_view, std::string_view>> getProperties() const noexcept;

    [[nodiscard]] std::pair<std::string_view, std::string_view> getNamespace() const noexcept;

    [[nodiscard]] std::expected<Node, Error> addChild(std::string_view) const noexcept;

    void addValue(std::string_view value) const;

    void addNamespace(std::string_view prefix, std::string_view uri) const;

    void removeNamespace() const;
};
} // namespace cpplibxml2
