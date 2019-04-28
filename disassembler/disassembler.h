#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <sstream>

std::string disassemble(uint8_t firstbyte, uint8_t lastbyte);
void disassemble_buffer(uint8_t *codebuffer, int pc);
