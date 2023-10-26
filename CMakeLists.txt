cmake_minimum_required(VERSION 3.16)
project(gcgp)

# Test if GCGP is build directly or via add_subdirectory
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(GCGP_IS_ROOT_PROJECT ON)
endif()

# option(GCGP_BUILD_TESTS "Build tests" ${GCGP_IS_ROOT_PROJECT})
option(GCGP_BUILD_EXAMPLES "Build examples" ${GCGP_IS_ROOT_PROJECT})

# The library
add_library(gcgp STATIC
    src/Command.cpp
    src/GCGP.cpp
    src/GrblInterface.cpp
    src/TokenizedCommand.cpp
)
add_library(gcgp::gcgp ALIAS gcgp)
target_include_directories(gcgp PUBLIC src)

# C++11 support without compiler-specific features
target_compile_features(gcgp PUBLIC cxx_std_11)
set_target_properties(gcgp PROPERTIES CXX_EXTENSIONS OFF)

# Download and link the dependencies
include(FetchContent)
FetchContent_Declare(glm
                     GIT_REPOSITORY https://github.com/g-truc/glm
                     GIT_TAG        0.9.9.8)
FetchContent_MakeAvailable(glm)
target_link_libraries(gcgp PUBLIC glm)

# Define unit tests
# if(GCGP_BUILD_TESTS)
#     enable_testing()
#     add_subdirectory(tests)
# endif()

# Define examples
if(GCGP_BUILD_EXAMPLES)
    add_subdirectory(examples-desktop)
endif()
