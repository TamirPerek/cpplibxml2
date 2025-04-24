#include "cpplibxml2.hpp"

#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include <iostream>
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

    int test(const std::filesystem::path &path)
    {
        // Initialize the library and check potential ABI mismatches
        LIBXML_TEST_VERSION

        // Note: The path is not used in this example, but you can use it to load different XML files.
        // For example: auto doc = xmlReadFile(path.c_str(), nullptr, 0);
        // Here we are using a hardcoded path for demonstration purposes.
        auto doc = xmlDocPtr_t(xmlReadFile(path.string().c_str(), nullptr, 0));

        if (doc == nullptr)
        {
            std::cerr << "Document not parsed successfully. \n";
            return -1;
        }

        auto root_node = xmlDocGetRootElement(doc.get());
        if (root_node == nullptr)
        {
            std::cerr << "Empty document\n";
            return -1;
        }
        

        return 0;
    }
} // namespace cpplibxml2