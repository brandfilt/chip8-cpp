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
    
Building disassembler and assember is done similarly.

# Running the emulator

The emulator executable takes the path to a Chip-8 ROM file as the only
argument.

    ./emulator INVADERS
    
The Chip-8 HEX keys are mapped to the corresponding characters A-F and 0-9.

Step mode can be enabled by pressing P. In step mode the emulator only advances
(reads next opcode) when the user presses SPACE.

When the emulator exits, it prints a JSON with the values of index register,
stack register, program counter and the registers V0-VF.

# Screenshots

![alt text](screenshots/invaders01.png?raw=true "Space invaders")

