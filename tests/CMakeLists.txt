
message(STATUS "Fetching Catch2 library...")
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)
FetchContent_MakeAvailable(Catch2)
message(STATUS "Fetching Catch2 library... Done")

add_executable(tokenize tokenize.cpp)
target_compile_features(tokenize PRIVATE cxx_std_20)
target_link_libraries(tokenize PRIVATE gcgp::gcgp Catch2::Catch2WithMain)
add_test(NAME tokenize COMMAND $<TARGET_FILE:tokenize>)
