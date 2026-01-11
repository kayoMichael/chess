cmake_minimum_required(VERSION 4.1)
project(chess)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(src)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(chess ${SOURCES}
        src/chess_piece/queen.cpp
        src/chess_piece/bishop.cpp
        src/chess_piece/rook.cpp
        src/chess_piece/king.cpp
        src/chess_piece/pawn.cpp)