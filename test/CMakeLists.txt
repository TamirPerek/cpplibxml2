# Set the project name
project(cpplibxml2_test)

include(GoogleTest)

# Enable testing
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add the test executable
add_executable(${PROJECT_NAME}
        DocClassTest.cpp
        helper.hpp
        ParserOptionsTest.cpp
        ErrorTypesTest.cpp
        NodeClassTest.cpp
        NodeNamespaceTest.cpp)

# Link GoogleTest and pthread
target_link_libraries(${PROJECT_NAME}
    PRIVATE gtest_main
    PRIVATE cpplibxml2
    PRIVATE LibXml2::LibXml2
)

if (WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/fsanitize=address>
    )
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>
    )

    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address>
    )
endif ()

add_custom_command(
    TARGET ${PROJECT_NAME} 
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/testData $<TARGET_FILE_DIR:${PROJECT_NAME}>/testData
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/testData ${CMAKE_CURRENT_BINARY_DIR}/testData
)

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying libxml2.dll..."
            COMMAND ${CMAKE_COMMAND} -E echo "From: $<TARGET_FILE:LibXml2::LibXml2>"
            COMMAND ${CMAKE_COMMAND} -E echo "To:   $<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:LibXml2::LibXml2>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

include(${CMAKE_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_NAME})

# Add the test
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

