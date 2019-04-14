cmake_minimum_required(VERSION 3.10.2)
project(Chip8Assembler)

add_definitions("-std=c++17 -Wall -pedantic")

add_executable(assembler assembler.cpp)
