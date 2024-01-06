cmake_minimum_required(VERSION 3.16)
project(gcgp)

option(GCGP_BUILD_TESTS "Build tests" OFF)

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
    src/tokenize.cpp
)
add_library(gcgp::gcgp ALIAS gcgp)
target_include_directories(gcgp PUBLIC src)

target_compile_features(gcgp PUBLIC cxx_std_17)
set_target_properties(gcgp PROPERTIES CXX_EXTENSIONS OFF)

# Enable exceptions
if (MSVC)
    target_compile_options(gcgp PRIVATE /EHsc)
else ()
    target_compile_options(gcgp PRIVATE -fexceptions)
endif ()

# Download and link the dependencies
message(STATUS "Fetching glm library...")
include(FetchContent)
FetchContent_Declare(glm
                     GIT_REPOSITORY https://github.com/g-truc/glm
                     GIT_TAG        master)
FetchContent_MakeAvailable(glm)
message(STATUS "Fetching glm library... Done")
target_link_libraries(gcgp PUBLIC glm::glm)

if (GCGP_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif ()

# Define examples
if(GCGP_BUILD_EXAMPLES)
    add_subdirectory(examples-desktop)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
