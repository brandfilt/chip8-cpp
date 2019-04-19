#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <stdint.h>
#include <string>

std::vector<std::string> split(const std::string &input,
                               const std::string &regex) {
  std::regex re(regex);
  std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;

  return {first, last};
}

uint16_t hex_literal_to_integer(const std::string &hex_literal) {
  std::stringstream string_stream;
  string_stream << std::hex << hex_literal.substr(1, 3);

  uint16_t value;
  string_stream >> value;

  return value;
}

std::vector<uint8_t> parse_register_numbers(const std::string &input) {
  std::regex re("V(\\d+),?");
  std::sregex_iterator first{input.begin(), input.end(), re}, last;

  std::vector<uint8_t> registers;
  for (auto i = first; i != last; ++i) {
    std::smatch match = *i;
    if (match.size() == 2) {
      std::ssub_match submatch = match[1];
      registers.push_back(std::stoi(submatch.str()));
    }
  }

  return registers;
}

uint16_t assemble_chip8(const std::string &command) {
  std::vector<std::string> tokens = split(command, "\\s+");
  std::string cmd = tokens[1];

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
  } else if (cmd == "SE") {
    std::vector<uint8_t> registers = parse_register_numbers(command);

    if (registers.size() == 1) {        // SE Vx, byte
      uint8_t byte = hex_literal_to_integer(tokens[3]);
      return (0x3000 | registers[0] << 8 | byte);
    } else if (registers.size() == 2) { // SE Vx, Vx
      return (0x5000 | registers[0] << 8 | registers[1] << 4);
    }

  } else if (cmd == "SNE") {
    std::vector<uint8_t> registers = parse_register_numbers(command);

    if (registers.size() == 1) {        // SNE Vx, byte
      uint8_t byte = hex_literal_to_integer(tokens[3]);
      return (0x4000 | registers[0] << 8 | byte);
    } else if (registers.size() == 2) { // SNE Vx, Vx
      return (0x9000 | registers[0] << 8 | registers[1] << 4);
    }
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
