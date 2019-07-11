#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <iterator>
#include <map>
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
      uint8_t byte = hex_literal_to_integer(tokens[2]);
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
      if (tokens[1] == "DT") {
        return (0xF015 | registers[0] << 8);
      } else if (tokens[1] == "ST") {
        return (0xF018 | registers[0] << 8);
      } else if (tokens[1] == "F") {
        return (0xF029 | registers[0] << 8);
      } else if (tokens[1] == "B") {
        return (0xF033 | registers[0] << 8);
      } else if (tokens[1] == "[I]") {
        return (0xF055 | registers[0] << 8);
      } else if (tokens[1] == "HF") {
        return (0xF030 | registers[0] << 8);
      } else if (tokens[1] == "R") {
        return (0xF075 | registers[0] << 8);
      } else if (tokens[2] == "DT") {
        return (0xF007 | registers[0] << 8);
      } else if (tokens[2] == "K") {
        return (0xF00A | registers[0] << 8);
      } else if (tokens[2] == "[I]") {
        return (0xF065 | registers[0] << 8);
      } else if (tokens[2] == "R") {
        return (0xF085 | registers[0] << 8);
      } else {
        uint8_t byte = hex_literal_to_integer(tokens[2]);
        return (0x6000 | registers[0] << 8 | byte);
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
  } else if (cmd == "RND") {
    std::vector<uint8_t> registers = parse_register_numbers(command);

    uint8_t byte = hex_literal_to_integer(tokens[2]);
    return (0xC000 | registers[0] << 8 | byte);
  } else if (cmd == "DRW") {
    std::vector<uint8_t> registers = parse_register_numbers(command);

    uint8_t byte = hex_literal_to_integer(tokens[3]);
    return (0xD000 | registers[0] << 8 | registers[1] << 4 | (byte & 0x0F));
  } else if (cmd == "SKP") {
    std::vector<uint8_t> registers = parse_register_numbers(command);
    return (0xE09E | registers[0] << 8);
  } else if (cmd == "SKNP") {
    std::vector<uint8_t> registers = parse_register_numbers(command);
    return (0xE0A1 | registers[0] << 8);
  } else if (cmd == "SCD") {
    std::vector<uint8_t> registers = parse_register_numbers(command);
    return (0x00C0 | (registers[0] & 0x0F));
  } else if (cmd == "SCR") {
    return 0x00FB;
  } else if (cmd == "SCL") {
    return 0x00FC;
  } else if (cmd == "EXIT") {
    return 0x00FD;
  } else if (cmd == "LOW") {
    return 0x00FE;
  } else if (cmd == "HIGH") {
    return 0x00FF;
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

  std::map<std::string, uint16_t> label_addresses;
  std::vector<std::string> lines;
  std::string line;
  int row = 0;
  while (std::getline(program, line)) {
    bool isEmpty = std::all_of(line.begin(), line.end(), [](char c) {
      return std::isspace(static_cast<unsigned char>(c));
    });

    if (isEmpty) continue;

    std::regex re("(\\w+:)");
    std::smatch match;
    if (std::regex_search(line, match, re)) {
      std::string label_match = match[1].str();
      std::string label = label_match.substr(0, label_match.size()-1);
      label_addresses.insert({label, row + 0x200});

      std::stringstream ss;
      ss << "^" << label_match << "\\s+";
      std::string remove = ss.str();

      line = std::regex_replace(line, std::regex(remove), "");
    }

    lines.push_back(line);
    row++;
  }

  for (auto line : lines) {
    std::smatch match;
    for (auto const& label : label_addresses) {
      if (std::regex_search(line, match, std::regex(label.first))) {
        // line = std::regex_replace()
      }
    }
    uint16_t instruction = assemble_chip8(line);
    output.put(instruction >> 8);
    output.put(instruction);
  }

  return 0;
}
