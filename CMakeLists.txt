cmake_minimum_required(VERSION 4.0)
project(tetris_cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(tetris_cpp
        main.cpp
        src/game.cpp
        src/board.cpp
        src/platform.cpp
        src/tetromino.cpp
)

target_include_directories(tetris_cpp PRIVATE include)