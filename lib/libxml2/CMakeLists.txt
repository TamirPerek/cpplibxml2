# FetchContent module to download libxml2 from GitHub
include(FetchContent)

FetchContent_Declare(
    libxml2
    GIT_REPOSITORY https://github.com/GNOME/libxml2.git
        GIT_TAG v2.15.0 # Replace with the desired version tag
)

set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
set(LIBXML2_WITH_ZLIB OFF CACHE BOOL "")
set(LIBXML2_WITH_LZMA OFF CACHE BOOL "")
set(LIBXML2_WITH_ICONV OFF CACHE BOOL "")
set(LIBXML2_WITH_TESTS OFF CACHE BOOL "")
set(LIBXML2_WITH_PYTHON OFF CACHE BOOL "")
set(LIBXML2_WITH_PROGRAMS OFF CACHE BOOL "")

FetchContent_MakeAvailable(libxml2)
