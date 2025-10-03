# cpplibxml2

**cpplibxml2** is a modern C++23 library providing a safe and convenient wrapper around [libxml2](http://xmlsoft.org/).
It makes it easy to parse, manipulate and serialize XML documents in C++ projects.

## Features

- Fully C++23-compatible
- Lightweight wrappers for libxml2 APIs
- Cross-platform: Windows, Linux, macOS
- CMake build system for easy integration
- Automated tests and continuous integration

## Prerequisites

- A C++23-capable compiler
    - GCC ≥ 10
    - Clang ≥ 11
    - MSVC 2019 or newer
- CMake ≥ 3.15

On Debian/Ubuntu, for example:

```bash 
sudo apt-get update sudo apt-get install build-essential cmake
```

## Build & Installation

1. Clone the repository
   ```bash
   git clone https://github.com/TamirPerek/cpplibxml2.git
   cd cpplibxml2
   ```
2. Create and configure the build directory
   ```bash
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   ```
3. Compile and install
   ```bash
   cmake --build . --config Release
   cmake --install . --prefix /desired/install/path
   ```

## Usage Example

```cpp 
#include <cpplibxml2.hpp> 
#include <print>

int main() { 
    // Load an XML document from file 
    auto doc = cpplibxml2::Document::loadFromFile("example.xml");
    
    // Access the root node
    auto root = doc.root();
    std::println("Root element: {}", root.name());
    
    // Read an attribute
    if (root.hasAttribute("id")) {
        std::println("ID: {}", root.attribute("id").asString());
    }

    // Serialize back to string
    std::println("{}", doc.toString());
    return 0;
}
```

> Note: Class and method names may vary by version. See the `include/` directory for the current API.

## Running Tests

From the build directory:

```bash 
ctest --output-on-failure
```

## Continuous Integration

This project uses GitHub Actions with a matrix build covering:

- Ubuntu + GCC
- Ubuntu + Clang
- Windows + MSVC

See the workflow file at `.github/workflows/ci.yml`.

![Build Status](https://github.com/YOUR_USERNAME/cpplibxml2/actions/workflows/ci.yml/badge.svg)

## Contributing

Contributions, bug reports and feature requests are welcome!

1. Open an issue or discuss your idea
2. Clone the repository and create a feature branch
3. Submit a pull request

Please follow the coding style and add tests for new functionality.

## License

This project is licensed under the [MIT License](LICENSE).
