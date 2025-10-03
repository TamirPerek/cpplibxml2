#pragma once

#include "errorTypes.hpp"

#include <expected>
#include <filesystem>
#include <memory>
#include <type_traits>
#include <vector>

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
                                           the tree allowed afterward (will possibly
                                           crash if you try to modify the tree) */
    Old10 = 1 << 17,          /* parse using XML-1.0 before update 5 */
    NoBaseFix = 1 << 18,      /* do not fixup XINCLUDE xml:base uris */
    Huge = 1 << 19,           /* relax any hardcoded limit from the parser */
    OldSax = 1 << 20,         /* parse using SAX2 interface before 2.7.0 */
    IgnoreEnc = 1 << 21,      /* ignore internal document encoding hint */
    BigLines = 1 << 22,       /* Store big lines numbers in the text PSVI field since 2.13.0 */
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

/**
 * Converts a Format enum value to its corresponding string representation.
 *
 * This function translates the given Format enum value into a readable
 * string that represents the encoding standard.
 *
 * @param format The Format enum value to be converted
 * @return A string representing the encoding ("UTF-8", "UTF-16", "ISO-8859-1", or "ASCII")
 * @note For Format::UTF_8 or unknown values, "UTF-8" is returned by default
 */
[[nodiscard]] inline std::string to_string(const Format format)
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

    [[nodiscard]] static std::expected<Doc, RuntimeError> parseFile(
        const std::filesystem::path &, ParserOptions options = ParserOptions::NoEnt | ParserOptions::DtdLoad) noexcept;

    [[nodiscard]] static std::expected<Doc, RuntimeError> parse(std::string_view,
                                                                ParserOptions = ParserOptions::NoEnt |
                                                                                ParserOptions::DtdLoad) noexcept;

    [[nodiscard]] std::expected<Node, RuntimeError> root() const noexcept;

    [[nodiscard]] std::expected<std::string, RuntimeError> dump(bool addWhiteSpaces = false,
                                                                Format format = Format::UTF_8) const noexcept;

    /**
     * Writes the XML document to the given file path.
     *
     * @param path The file system path where the document should be saved
     * @param addWhiteSpaces If true, adds indentation and line breaks
     * @param format Encoding format (UTF-8 by default)
     * @return Success or Error
     */
    [[nodiscard]] std::expected<void, RuntimeError> saveToFile(const std::filesystem::path &path,
                                                               bool addWhiteSpaces = false,
                                                               Format format = Format::UTF_8) const noexcept;
};

class Node
{
    Node();

    struct Impl;
    std::unique_ptr<Impl> impl;

    friend class Doc;

  public:
    Node(const Node &) = delete;

    Node(Node &&) noexcept;

    ~Node();

    Node &operator=(const Node &) = delete;

    Node &operator=(Node &&) noexcept;

    [[nodiscard]] std::expected<std::string_view, RuntimeError> name() const noexcept;

    [[nodiscard]] std::expected<Node, RuntimeError> findChild(std::string_view name) const noexcept;

    [[nodiscard]] std::expected<Node, RuntimeError> findChild(std::string_view name,
                                                              std::string_view nsUri) const noexcept;

    [[nodiscard]] std::expected<std::vector<Node>, RuntimeError> getChildren() const noexcept;

    [[nodiscard]] std::expected<std::string, RuntimeError> value() const noexcept;


    /**
     * Retrieve the node’s text content and convert it to a float.
     * Internally calls std::stof and wraps any std::invalid_argument
     * or std::out_of_range exceptions into an InvalidArgument error.
     *
     * @return std::expected<float, InvalidArgument>
     *         - contains the parsed float value on success
     *         - contains an InvalidArgument error if conversion fails
     */
    [[nodiscard]] std::expected<float, InvalidArgument> valueAsFloat() const;
    /**
     * Retrieve the node’s text content and convert it to a double.
     * Internally calls std::stod and wraps any std::invalid_argument
     * or std::out_of_range exceptions into an InvalidArgument error.
     *
     * @return std::expected<double, InvalidArgument>
     *         - contains the parsed double value on success
     *         - contains an InvalidArgument error if conversion fails
     */
    [[nodiscard]] std::expected<double, InvalidArgument> valueAsDouble() const;
    /**
     * Retrieve the node’s text content and convert it to an int.
     * Internally calls std::stoi and wraps any std::invalid_argument
     * or std::out_of_range exceptions into an InvalidArgument error.
     *
     * @return std::expected<int, InvalidArgument>
     *         - contains the parsed int value on success
     *         - contains an InvalidArgument error if conversion fails
     */
    [[nodiscard]] std::expected<int, InvalidArgument> valueAsInt(int base = 10) const;
    /**
     * Retrieve the node’s text content and convert it to a long.
     * Internally calls std::stol and wraps any std::invalid_argument
     * or std::out_of_range exceptions into an InvalidArgument error.
     *
     * @return std::expected<long, InvalidArgument>
     *         - contains the parsed long value on success
     *         - contains an InvalidArgument error if conversion fails
     */
    [[nodiscard]] std::expected<long, InvalidArgument> valueAsLong(int base = 10) const;
    /**
     * Retrieve the node’s text content and convert it to a long long.
     * Internally calls std::stoll and wraps any std::invalid_argument
     * or std::out_of_range exceptions into an InvalidArgument error.
     *
     * @return std::expected<long long, InvalidArgument>
     *         - contains the parsed long long value on success
     *         - contains an InvalidArgument error if conversion fails
     */
    [[nodiscard]] std::expected<long long, InvalidArgument> valueAsLongLong(int base = 10) const;

    [[nodiscard]] std::expected<std::pair<std::string_view, std::string_view>, RuntimeError> findProperty(
        const std::string_view name) const noexcept;

    [[nodiscard]] std::vector<std::pair<std::string_view, std::string_view>> getProperties() const noexcept;

    [[nodiscard]] std::pair<std::string_view, std::string_view> getNamespace() const noexcept;

    [[nodiscard]] std::expected<Node, RuntimeError> addChild(std::string_view) const noexcept;

    void addValue(std::string_view value) const;

    void addNamespace(std::string_view prefix, std::string_view uri) const;

    void removeNamespace() const;
};
} // namespace cpplibxml2