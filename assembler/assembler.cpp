#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <stdint.h>
#include <string>

std::vector<std::string> tokenize(const std::string &input) {
  std::regex re("([A-z0-9#]+)");
  std::sregex_iterator first{input.begin(), input.end(), re}, last;

  std::vector<std::string> tokens;
  for (auto i = first; i != last; ++i) {
    std::smatch match = *i;
    if (match.size() == 2) {
      std::ssub_match submatch = match[1];
      tokens.push_back(submatch.str());
    }
  }

  return tokens;
}

std::vector<std::string> split(const std::string &input,
                               const std::string &regex) {
  std::regex re(regex);
  std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;

  return {first, last};
}

uint16_t hex_literal_to_integer(const std::string &hex_literal) {
  std::stringstream string_stream;
  string_stream << std::hex << hex_literal.substr(1, hex_literal.size());

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
  // std::vector<std::string> tokens = split(command, "\\s+");

  std::vector<std::string> tokens = tokenize(command);
  std::string cmd = tokens[0];

  if (cmd == "CLS") {
    return 0x00E0;
  } else if (cmd == "RET") {
    return 0x00EE;
  } else if (cmd == "JP") {
    std::vector<uint8_t> registers = parse_register_numbers(command);
    if (registers.size() == 0) {
      uint16_t addr = hex_literal_to_integer(tokens[1]);
      return (0x1000 | addr);
    } else {
      uint16_t addr = hex_literal_to_integer(tokens[2]);
      return (0xB000 | addr);
    }
  } else if (cmd == "CALL") {
    uint16_t addr = hex_literal_to_integer(tokens[1]);
    return (0x2000 | addr);
  } else if (cmd == "SE") {
    std::vector<uint8_t> registers = parse_register_numbers(command);

    if (registers.size() == 1) { // SE Vx, byte
      uint8_t byte = hex_literal_to_integer(tokens[1]);
      return (0x3000 | registers[0] << 8 | byte);
    } else if (registers.size() == 2) { // SE Vx, Vx
      return (0x5000 | registers[0] << 8 | registers[1] << 4);
    }

  } else if (cmd == "SNE") {
    std::vector<uint8_t> registers = parse_register_numbers(command);

    if (registers.size() == 1) { // SNE Vx, byte
      uint8_t byte = hex_literal_to_integer(tokens[2]);
      return (0x4000 | registers[0] << 8 | byte);
    } else if (registers.size() == 2) { // SNE Vx, Vx
      return (0x9000 | registers[0] << 8 | registers[1] << 4);
    }
  } else if (cmd == "LD") {
    std::vector<uint8_t> registers = parse_register_numbers(command);

    if (registers.size() == 1) {
      if (tokens[2] == "DT") {
        return (0xF015 | registers[0] << 8);
      } else if (tokens[2] == "ST") {
        return (0xF018 | registers[0] << 8);
      } else if (tokens[2] == "F") {
        return (0xF029 | registers[0] << 8);
      } else if (tokens[2] == "B") {
        return (0xF033 | registers[0] << 8);
      } else if (tokens[2] == "[I]") {
        return (0xF055 | registers[0] << 8);
      } else if (tokens[2] == "HF") {
        return (0xF030 | registers[0] << 8);
      } else if (tokens[2] == "R") {
        return (0xF075 | registers[0] << 8);
      } else if (tokens[3] == "DT") {
        return (0xF007 | registers[0] << 8);
      } else if (tokens[3] == "K") {
        return (0xF00A | registers[0] << 8);
      } else if (tokens[3] == "[I]") {
        return (0xF065 | registers[0] << 8);
      } else if (tokens[3] == "R") {
        return (0xF085 | registers[0] << 8);
      }
    } else if (registers.size() == 2) {
      return (0x8000 | registers[0] << 8 | registers[1] << 4);
    } else if (registers.size() == 0) {
      uint16_t addr = hex_literal_to_integer(tokens[2]);
      return (0xA000 | addr);
    }
  } else if (cmd == "ADD") {
    std::vector<uint8_t> registers = parse_register_numbers(command);

    if (registers.size() == 1) {
      if (tokens[1] == "I") {
        return (0xF01E | registers[0] << 8);
      } else {
        uint8_t byte = hex_literal_to_integer(tokens[2]);
        return (0x7000 | registers[0] << 8 | byte);
      }
    } else if (registers.size() == 2) {
      return (0x8004 | registers[0] << 8 | registers[1] << 4);
    }
  } else if (cmd == "OR") {
    std::vector<uint8_t> registers = parse_register_numbers(command);
    return (0x8001 | registers[0] << 8 | registers[1]);
  } else if (cmd == "AND") {
    std::vector<uint8_t> registers = parse_register_numbers(command);
    return (0x8002 | registers[0] << 8 | registers[1]);
  } else if (cmd == "XOR") {
    std::vector<uint8_t> registers = parse_register_numbers(command);
    return (0x8003 | registers[0] << 8 | registers[1]);
  } else if (cmd == "SUB") {
    std::vector<uint8_t> registers = parse_register_numbers(command);
    return (0x8005 | registers[0] << 8 | registers[1]);
  } else if (cmd == "SHR") {
    std::vector<uint8_t> registers = parse_register_numbers(command);
    if (registers.size() == 1) {
      return (0x8006 | registers[0] << 8);
    } else {
      return (0x8006 | registers[0] << 8 | registers[1]);
    }
  } else if (cmd == "SUBN") {
    std::vector<uint8_t> registers = parse_register_numbers(command);
    return (0x8007 | registers[0] << 8 | registers[1]);
  } else if (cmd == "SHL") {
    std::vector<uint8_t> registers = parse_register_numbers(command);
    if (registers.size() == 1) {
      return (0x800E | registers[0] << 8);
    } else {
      return (0x800E | registers[0] << 8 | registers[1]);
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
