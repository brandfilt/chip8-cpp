#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <stdint.h>

std::vector<std::string> split(const std::string &input, const std::string &regex) {
  std::regex re(regex);
  std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;

  return {first, last};
}

void assemble_chip8(std::string &command) {
  std::vector<std::string> tokens = split(command, "\\s+");
  std::cout << tokens[0] << std::endl;
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
    assemble_chip8(line);
  }

  return 0;
}
