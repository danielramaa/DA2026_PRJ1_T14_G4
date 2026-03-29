cmake_minimum_required(VERSION 3.10)
project(DA2026_PRJ1_T14_G3)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(DA2026_PRJ1_T14_G3
        src/main.cpp
        include/types.h
        include/graph.h
        include/parser.h
        src/parser.cpp)