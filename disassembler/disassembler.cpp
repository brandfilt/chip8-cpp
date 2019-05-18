#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <sstream>

std::string disassemble(uint8_t firstbyte, uint8_t lastbyte) {
  std::string output;
  std::stringstream output_stream;

  uint8_t firstword = (firstbyte >> 4);

  switch (firstword) {
  case 0x00:
    switch (lastbyte) {
    case 0xE0: {
      // 00E0
      // CLS
      // Clear the screen
      output_stream << "CLS";
      std::cout << output;
    } break;
    case 0xEE: {
      // 00EE
      // RTS
      // Return from subroutine
      output_stream << "RET";
    } break;
    case 0xFD: {
      // 00EE
      // RTS
      // Return from subroutine
      output_stream << "EXIT";
    } break;
    }
    break;
  case 0x01: {
    // JP addr
    // Jump to address NNN
    uint8_t addresshi = firstbyte & 0x0F;
    output_stream << "JP #" << std::hex << std::setw(1) << static_cast<int>(addresshi)
              << std::setw(2) << static_cast<int>(lastbyte);
  } break;
  case 0x02: {
    // CALL addr
    // Jump to subroutine at NNN
    uint8_t addresshi = firstbyte & 0x0F;
    output_stream << "CALL #" << std::hex << ((addresshi << 8) | (lastbyte));
  } break;
  case 0x03: {
    // SE Vx, addr
    // Skip the next instruction if VX equals NN
    uint8_t addresshi = firstbyte & 0x0F;
    output_stream << "SE V" << std::setw(1) << static_cast<int>(addresshi)
              << ", #" << std::setw(2) << static_cast<int>(lastbyte);
  } break;
  case 0x04: {
    // SNE Vx, addr
    // Skip the next instruction if VX doesn't equal NN
    uint8_t addresshi = firstbyte & 0x0F;
    output_stream << "SNE V" << std::setw(1) << static_cast<int>(addresshi)
              << ", #" << std::setw(2) << static_cast<int>(lastbyte);
  } break;
  case 0x05: {
    // SE Vx, Vy
    // Skip the next instruction if VX equals VY
    uint8_t firstaddress = firstbyte & 0x0F;
    uint8_t secondaddress = (lastbyte & 0xF0) >> 4;
    output_stream << "SE V" << std::setw(1) << static_cast<int>(firstaddress)
              << ", V" << std::setw(1) << static_cast<int>(secondaddress);
  } break;
  case 0x06: {
    // LD Vx, addr
    // Sets register VX to NN
    uint8_t reg = firstbyte & 0x0F;
    output_stream << "LD V" << std::setw(1) << static_cast<int>(reg) << ", #"
                  << std::hex << std::setw(2) << static_cast<int>(lastbyte);
  } break;
  case 0x07: {
    // ADD Vx, addr
    // Add NN to value in register VX
    uint8_t reg = firstbyte & 0x0F;
    output_stream << "ADD V" << std::setw(1) << static_cast<int>(reg) << ", #"
              << std::setw(2) << static_cast<int>(lastbyte);
  } break;
  case 0x08: {
    // 8XYN have 9 different operations using two registers
    uint8_t operation = lastbyte & 0x0F;
    uint8_t regx = firstbyte & 0x0F;
    uint8_t regy = (lastbyte & 0xF0) >> 4;
    switch (operation) {
    case 0x00: {
      // LD Vx, Vy
      // Set VX to the value of VY
      output_stream << "LD V" << std::setw(1) << static_cast<int>(regx) << ", V"
                << std::setw(1) << static_cast<int>(regy);
    } break;
    case 0x01: {
      // OR Vx, Vy
      // Set VX to VX OR VY
      output_stream << "OR V" << std::setw(1) << static_cast<int>(regx) << ", V"
                << std::setw(1) << static_cast<int>(regy);
    } break;
    case 0x02: {
      // AND Vx, Vy
      // Set VX to VX AND VY
      output_stream << "AND V" << std::setw(1) << static_cast<int>(regx) << ", V"
                << std::setw(1) << static_cast<int>(regy);
    } break;
    case 0x03: {
      // XOR Vx, Vy
      // Set VX to VX XOR VY
      output_stream << "XOR V" << std::setw(1) << static_cast<int>(regx) << ", V"
                << std::setw(1) << static_cast<int>(regy);
    } break;
    case 0x04: {
      // ADD Vx, Vy
      // Add VX to VY, VF will be set to 1 if there's a carry and to 0 if
      // there's not
      output_stream << "ADD V" << std::setw(1) << static_cast<int>(regx) << ", V"
                << std::setw(1) << static_cast<int>(regy);
    } break;
    case 0x05: {
      // SUB Vx, Vy
      // Substract VY from VX. VF is set to 0 if there's a borrow, and to 1 if
      // there's not.
      output_stream << "SUB V" << std::setw(1) << static_cast<int>(regx) << ", V"
                << std::setw(1) << static_cast<int>(regy);
    } break;
    case 0x06: {
      // SHR Vx
      // Shift VX right by one. VF is set to the value of th least significant
      // bit of VX before the shift.
      output_stream << "SHR V" << std::setw(1) << static_cast<int>(regx);
    } break;
    case 0x07: {
      // 8XY7
      // SUBN Vx, Vy
      // Substract VY from VX. VF is set to the mos significant bit of VX before
      // shift.
      output_stream << "SUBN V" << static_cast<int>(regx) << ", V"
                << static_cast<int>(regy);
    } break;
    case 0x0e: {
      // 8XYe
      // SHL Vx
      // Shifts VX left by one. VF is set to the value of the most significant
      // bit of VX before shift.
      output_stream << "SHL V" << static_cast<int>(regx);
    } break;
    }
  } break;
  case 0x09: {
    // 9XY0
    // SNE Vx, Vy
    // Skip the next instruction if VX doesn't equal VY
    uint8_t firstaddress = firstbyte & 0x0F;
    uint8_t secondaddress = (lastbyte & 0xF0) >> 4;
    output_stream << "SNE V" << std::setw(1) << static_cast<int>(firstaddress)
              << ", V" << std::setw(1) << static_cast<int>(secondaddress);
  } break;
  case 0x0a: {
    // ANNN
    // LD I, addr
    // Sets I (index register) to the adress of NNN
    uint8_t addresshi = firstbyte & 0x0F;
    output_stream << "LD I, #" << std::hex << std::setw(1) << static_cast<int>(addresshi)
              << std::setw(2) << static_cast<int>(lastbyte);

  } break;
  case 0x0b: {
    // BNNN
    // JP V0, addr
    // Jumps to address NNN plus value o V0
    uint8_t addresshi = firstbyte & 0x0F;
    output_stream << "JP V0, " << std::setw(1) << static_cast<int>(addresshi)
              << std::setw(2) << static_cast<int>(lastbyte);

  } break;
  case 0x0c: {
    // CXNN
    // RND Vx, byte
    // Sets VX to a random value plus NN
    uint8_t addresshi = firstbyte & 0x0F;
    output_stream << "RND V" << std::setw(1) << static_cast<int>(addresshi)
                  << ", #" << std::hex << std::setw(2) << static_cast<int>(lastbyte);

  } break;
  case 0x0d: {
    // DXYN
    // DRW Vx, Vy, addr
    // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and
    // height of N pixels starting from memory location I (index register).
    uint8_t height = lastbyte & 0x0F;
    uint8_t regx = firstbyte & 0x0F;
    uint8_t regy = (lastbyte & 0xF0) >> 4;
    output_stream << "DRW V" << static_cast<int>(regx) << ", V"
                  << static_cast<int>(regy) << ", #" << std::hex << static_cast<int>(height);

  } break;
  case 0x0e: {
    // Two instructions for key input handling
    uint8_t operation = lastbyte & 0xFF;
    uint8_t reg = lastbyte & 0x0F;
    switch (operation) {
    case 0x9E:
      // EX9E
      // SKP Vx
      // Skip the next instruction if key stored in VX is pressed
      output_stream << "SKP V" << static_cast<int>(reg);
      break;
    case 0xA1:
      // EXA1
      // SKNP Vx
      // Skip the next instruction if key stored in VX is not pressed
      output_stream << "SKNP V" << static_cast<int>(reg);
      break;
    }
  } break;
  case 0x0f: {
    uint8_t operation = lastbyte & 0xFF;
    uint8_t reg = firstbyte & 0x0F;
    switch (operation) {
    case 0x07:
      // FX07
      // LD Vx, DT
      // Set VX to the value of the delay timer
      output_stream << "LD V" << static_cast<int>(reg) << ", DT";
      break;
    case 0x0A:
      // FX07
      // LD Vx, K
      // Wait for key stored in VX to be pressed
      output_stream << "LD V" << static_cast<int>(reg) << ", K";
      break;
    case 0x15:
      // FX15
      // LD DT, V
      // Set delay timer to the value of VX
      output_stream << "LD DT, V" << static_cast<int>(reg);
      break;
    case 0x18:
      // FX18
      // LD ST, Vx
      // Set sound timer to the value of VX
      output_stream << "LD ST, V" << static_cast<int>(reg);
      break;
    case 0x1E:
      // FX1E
      // ADD I, Vx
      // Add value of VX to I (index register)
      output_stream << "ADD I, V" << static_cast<int>(reg);
      break;
    case 0x29:
      // FX29
      // LD F, Vx
      // Set I (index register) to the location of the sprite for character
      // stored in VX
      output_stream << "LD F, V" << static_cast<int>(reg);
      break;
    case 0x33:
      // FX33
      // LD B, Vx
      // Stores Binary-coded decimal representation of VX at I (index register)
      output_stream << "LD B, V" << static_cast<int>(reg);
      break;
    case 0x55:
      // FX55
      // LD [I], Vx
      // Stores V0 to VX in memory starting at I (index register)
      output_stream << "LD [I], V" << static_cast<int>(reg);
      break;
    case 0x65:
      // FX65
      // LD Vx, [I]
      // Fills V0 to VX with values from memory starting at address I
      output_stream << "LD V" << static_cast<int>(reg) << ", [I]";
      break;
    }
  }
  }

  return output_stream.str();
}

void disassemble_buffer(uint8_t *codebuffer, int pc) {
  uint8_t *code = &codebuffer[pc];

  uint8_t firstbyte = code[0];
  uint8_t lastbyte = code[1];

  std::cout << std::hex << std::setfill('0') << std::setw(4) << pc << " "
            << std::setw(2) << static_cast<int>(firstbyte) << " " << std::setw(2)
            << static_cast<int>(lastbyte) << " ";


  std::string result = disassemble(firstbyte, lastbyte);
  std::cout << result;
}
