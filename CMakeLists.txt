cmake_minimum_required(VERSION 3.27)

project(main)

set(CMAKE_CXX_STANDARD 20)

add_executable(prog main.cpp room.cpp)

add_subdirectory(tests)