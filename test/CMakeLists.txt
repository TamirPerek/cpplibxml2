cmake_minimum_required(VERSION 3.20)

# Set the project name
project(cpplibxml2_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

include(GoogleTest)

# Enable testing
enable_testing()

# Add the test executable
add_executable(${PROJECT_NAME}
        DocClassTest.cpp
        helper.hpp
        ParserOptionsTest.cpp
        ErrorTypesTest.cpp
        NodeClassTest.cpp)

# Link GoogleTest and pthread
target_link_libraries(${PROJECT_NAME} 
    PRIVATE gtest 
    PRIVATE gmock 
    PRIVATE gtest_main
    PRIVATE cpplibxml2
    PRIVATE LibXml2::LibXml2
)

add_custom_command(
    TARGET ${PROJECT_NAME} 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/testData ${CMAKE_CURRENT_BINARY_DIR}/testData
)

# Add the test
gtest_discover_tests(${PROJECT_NAME})

include(${CMAKE_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_NAME})
