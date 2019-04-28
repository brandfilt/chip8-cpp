cmake_minimum_required(VERSION 3.10.2)
project(Chip8Emulator)

add_definitions("-std=c++17 -Wall -pedantic")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(emulator emulator.cpp ../disassembler/disassembler.cpp)
target_link_libraries(emulator ${CONAN_LIBS})
