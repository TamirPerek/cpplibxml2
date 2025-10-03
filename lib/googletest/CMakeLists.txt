
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0 # Replace with the desired version tag
)

# Make the content available
FetchContent_MakeAvailable(googletest)

# Link GoogleTest to your project
enable_testing()