#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>

class chip8 {
public:
  chip8() {
    this->memory = new unsigned char[1024 * 4 + 0x200]; // 4K memory reserved
    this->SP = 0xfa0;
    this->PC = 0x200; // Programs are loaded at 0x200
  }

  void load_rom(char *filename) {
    std::ifstream rom(filename,
                      std::ios::in | std::ios::binary | std::ios::ate);

    if (!rom.is_open()) {
      std::cout << "Couldn't open file!" << std::endl;
    }

    int size = rom.tellg();

    // Load rom to memory at 0x200
    rom.seekg(0, std::ios::beg);
    rom.read((char *)this->memory + 0x200, size);
    rom.close();
  }

  void emulate() {
    // Fetch first 8 bits of next instruction
    uint8_t *op = &this->memory[this->PC];

    int highnib = (*op & 0xf0) >> 4;
    switch (highnib) {
    case 0x00:
      break;
    case 0x01: {
      // JUMP $NNN
      // Jump to NNN
      uint16_t target = ((op[0] & 0xf) << 8) | op[1];
      this->PC = target;
    } break;
    case 0x02: {
      // CALL $NNN
      // Call subroutine at NNN

      // Advance stack pointer
      this->SP = -2;

      // Store next instructions address to memory pointed by the stack pointer
      memory[this->SP] = ((this->PC + 2) & 0xff00) >> 8;
      memory[this->SP + 1] = ((this->PC + 2) & 0x00ff);

      // Jump to subroutines address NNN
      uint16_t target = ((op[0] & 0xf) << 8) | op[1];
      this->PC = target;
    } break;
    case 0x03: {
      uint8_t reg = op[0] & 0x0f;
      if (this->V[reg] == op[1])
        this->PC += 2;
      this->PC += 2;
    } break;
    case 0x04: {
      uint8_t reg = op[0] & 0x0f;
      if (this->V[reg] != op[1])
        this->PC += 2;
      this->PC += 2;
    } break;
    case 0x05: {
      uint8_t reg1 = op[0] & 0x0f;
      uint8_t reg2 = (op[1] & 0xf0) >> 4;
      if (this->V[reg1] == this->V[reg2])
        this->PC += 2;
      this->PC += 2;
    } break;
    case 0x06: {
      uint8_t reg = op[0] & 0x0f;
      this->V[reg] = op[1];
      this->PC += 2;
    } break;
    case 0x07: {
      uint8_t reg = op[0] & 0x0f;
      this->V[reg] += op[1];
      this->PC += 2;
    } break;
    case 0x08: {
      uint8_t command = op[1] & 0x0f;
      uint8_t reg1 = op[0] & 0x0f;
      uint8_t reg2 = (op[1] & 0xf0) >> 4;

      switch (command) {
      case 0x1: {
        this->V[reg1] |= this->V[reg2];
      } break;
      case 0x2: {
        this->V[reg1] &= this->V[reg2];
      } break;
      case 0x3: {
        this->V[reg1] ^= this->V[reg2];
      } break;
      case 0x4: {
        uint16_t result = this->V[reg1] + this->V[reg2];
        this->V[0xf] = (result & 0xff00) ? 1 : 0;
        this->V[reg1] = result & 0xff;
      } break;
      case 0x5: {
        uint8_t result = this->V[reg1] - this->V[reg2];
        this->V[0xf] = (result > 0) ? 1 : 0;
        this->V[reg1] = result;
      } break;
      case 0x6: {
        this->V[0xf] = this->V[reg1] & 0x1;
        this->V[reg1] = this->V[reg1] >> 1;
      } break;
      case 0x7: {
        uint8_t result = this->V[reg2] - this->V[reg1];
        this->V[0xf] = (result > 0) ? 1 : 0;
        this->V[reg1] = result;
      } break;
      case 0xe: {
        this->V[0xf] = (0x80 == (this->V[reg1] & 0x80));
        this->V[reg1] = this->V[reg1] << 1;
      } break;
      }
      this->PC += 2;
    } break;
    case 0x09: {
      uint8_t reg1 = op[0] & 0xf;
      uint8_t reg2 = (op[1] & 0xf0) >> 4;
      if (this->V[reg1] != this->V[reg2])
        this->PC += 2;
      this->PC += 2;
    }
    case 0x0a: {
      this->I = ((op[0] & 0xf) << 8) | op[1];
      this->PC += 2;
    } break;
    case 0x0b: {
      uint16_t num = ((op[0] & 0xf) << 8) | op[1];
      this->PC = this->V[0] + num;
    } break;
    case 0x0c: {
      uint8_t reg = op[0] & 0xf;
      uint8_t num = op[1];
      this->V[reg] = (rand() % 256) + num;
    } break;
    case 0x0d: {
      // TODO: Drawing sprites
    } break;
    case 0x0e: {
      // TODO: Keyboard handling
    } break;
    case 0x0f: {
      int reg = op[0] & 0xf;
      switch (op[1]) {
      case 0x07: {
        this->V[reg] = this->delay;
      } break;
      case 0x0A:
        // Not implemented yet
        break;
      case 0x15: {
        this->delay = this->V[reg];
      } break;
      case 0x18: {
        this->sound = this->V[reg];
      } break;
      case 0x1e: {
        this->I = this->V[reg] + this->I;
      } break;
      case 0x29:
        // Not implemented yet
        break;
      case 0x33: {
        /* Fx33 - LD B, Vx
         * Store Binary Coded Decimal representation of Vx in memory
         * locations I, I+1 and I+2.
         */
        uint8_t ones, tens, hundreds;
        uint8_t value = this->V[reg];
        ones = value % 10;
        value /= 10;
        tens = value % 10;
        hundreds = value / 10;
        this->memory[this->I] = hundreds;
        this->memory[this->I + 1] = tens;
        this->memory[this->I + 2] = ones;
      } break;
      case 0x55: {
        /* Fx55 - LD [I], Vx
         * Store registers V0 to Vx in memory starting at location I.
         */
        for (auto i = 0; i < reg; i++)
          this->memory[this->I + i] = this->V[reg + i];
      } break;
      case 0x65: {
        /* Fx65 - LD Vx, [I]
         * Read registers V0 to Vx from memory starting at location I.
         */
        for (auto i = 0; i < reg; i++)
          this->V[reg + i] = this->memory[this->I + i];
      } break;
      }
      this->PC += 2;
    } break;
    }
  }

private:
  uint8_t V[16]; // Registers 0-F
  uint16_t I;    // Index register
  uint16_t SP;   // Stack pointer
  uint16_t PC;   // Program counter
  uint8_t delay; // Delay timer
  uint8_t sound; // Sound timer
  uint8_t *memory;
  uint8_t *screen; // Same as memory[0xF00]
};

int main(int argc, char **argv) {
  chip8 vm;

  vm.load_rom(argv[1]);
  return 0;
}
