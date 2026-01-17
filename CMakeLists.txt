cmake_minimum_required(VERSION 3.28.2)
project(chess)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_library(chess_lib ${LIB_SOURCES})
target_include_directories(chess_lib PUBLIC include)

add_executable(chess src/main.cpp)
target_link_libraries(chess chess_lib)

enable_testing()
add_executable(chess_tests
        tests/test_board.cpp
        tests/test_pawn.cpp
)
target_link_libraries(chess_tests chess_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(chess_tests)