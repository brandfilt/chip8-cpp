#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <iterator>
#include <regex>
#include <stdint.h>
#include <string>

std::vector<std::string> split(const std::string &input,
                               const std::string &regex) {
  std::regex re(regex);
  std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;

  return {first, last};
}

uint16_t hex_literal_to_integer(const std::string hex_literal) {
  std::stringstream string_stream;
  string_stream << std::hex << hex_literal.substr(1, 3);

  uint16_t value;
  string_stream >> value;

  return value;
}

uint16_t assemble_chip8(const std::string &command) {
  std::vector<std::string> tokens = split(command, "\\s+");
  std::string cmd = tokens[1];

  for (auto const &value : tokens) {
    std::cout << "TOKEN: " << value << std::endl;
  }
  if (cmd == "CLS") {
    return 0x00E0;
  } else if (cmd == "RET") {
    return 0x00EE;
  } else if (cmd == "JP") {
    uint16_t addr = hex_literal_to_integer(tokens[2]);
    return (0x1000 | addr);
  } else if (cmd == "CALL") {
    uint16_t addr = hex_literal_to_integer(tokens[2]);
    return (0x2000 | addr);
  }

  return 0x0000;
}

int main(int argc, char **argv) {
  std::ifstream program(argv[1], std::ios::in);
  std::ofstream output(argv[2], std::ios::out | std::ios::binary);

  if (!program.is_open()) {
    std::cout << "Couldn't open source code file!" << std::endl;
    return 1;
  }

  if (!output.is_open()) {
    std::cout << "Couldn't open output file!" << std::endl;
    return 1;
  }

  std::string line;
  while (std::getline(program, line)) {
    uint16_t instruction = assemble_chip8(line);
    output.put(instruction >> 8);
    output.put(instruction);
  }

  return 0;
}
