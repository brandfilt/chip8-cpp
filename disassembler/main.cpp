#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <sstream>

#include "disassembler.h"

int main(int argc, char **argv) {
  std::ifstream rom(argv[1], std::ios::in | std::ios::binary | std::ios::ate);

  if (!rom.is_open()) {
    std::cout << "Couldn't open file!" << std::endl;
    return 1;
  }

  long size = rom.tellg();
  unsigned char *buffer = new unsigned char[size + 0x200];

  rom.seekg(0, std::ios::beg);
  rom.read((char *)buffer + 0x200, size);
  rom.close();

  int pc = 0x200;
  while (pc < (size + 0x200)) {
    disassemble_buffer(buffer, pc);
    pc += 2;
    std::cout << std::endl;
  }

  return 0;
}
