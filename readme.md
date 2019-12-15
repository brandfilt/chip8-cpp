# Chip-8 emulator, assembler and disassembler

Simple Chip-8 emulator written in somewhat modern C++. 

This project is not aiming to be a efficient emulation but more of a learning
experience. The CPU emulation is written basically as a big switch-case statement for readability.

# Building

Conan and CMake are used for dependency management and building.

    cd emulator
    mkdir build
    conan install ../conanfile.txt -s compiler.version=10.0
    cmake .
    make
    
Buildign disassembler and assember is done similarly.

# Screenshots

![alt text](screenshots/invaders01.png?raw=true "Space invaders")

