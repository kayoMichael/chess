cmake_minimum_required(VERSION 4.1)
project(chess)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(chess main.cpp
        piece/piece.h
        board/board.h
        board/board.cpp
        move/move.h
        piece/pawn.cpp)