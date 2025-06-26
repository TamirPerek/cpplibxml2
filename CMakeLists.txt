cmake_minimum_required(VERSION 3.16)

# Project name and version
project(cpplibxml2 VERSION 0.0.1 LANGUAGES CXX)

include(CTest)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_DEBUG_POSTFIX _d)

add_subdirectory(lib)

include_directories(include)

set(HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cpplibxml2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/errorTypes.hpp
)
set(MY_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

add_library(${PROJECT_NAME}_Warnings INTERFACE)

# Add source files
add_library(
        ${PROJECT_NAME}
        STATIC
)

message(STATUS "CXX compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CXX compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER}")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Adding Clang-specific compiler flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

target_sources(
        ${PROJECT_NAME}
        PRIVATE
        ${MY_SOURCE_FILES}
        PUBLIC
        ${HEADER_FILES}
)

# Include directories
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link library to the executable
target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC LibXml2::LibXml2
)

add_subdirectory(test)

include(cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_NAME}_Warnings)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_Warnings)
