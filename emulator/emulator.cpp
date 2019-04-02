#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>

#include <SDL.h>

#include "display.h"
#include "keyboard.h"

class Chip8 {
public:
  Chip8() {
    m_memory = new unsigned char[1024 * 4 + 0x200]; // 4K memory reserved
    m_SP = 0xfa0;
    m_PC = 0x200; // Programs are loaded at 0x200
  }

  void load_rom(char *filename) {
    std::ifstream rom(filename,
                      std::ios::in | std::ios::binary | std::ios::ate);

    if (!rom.is_open()) {
      std::cout << "Couldn't open file!" << std::endl;
      m_ready = false;
      return;
    }

    int size = rom.tellg();

    // Load rom to memory at 0x200
    rom.seekg(0, std::ios::beg);
    rom.read((char *)m_memory + 0x200, size);
    rom.close();

    m_ready = true;
  }

  void init() { int ret = m_display.init(); }

  void emulate() {
    if (!m_ready)
      return;
    // Fetch first 8 bits of next instruction
    uint8_t *op = &m_memory[m_PC];

    int highnib = (*op & 0xf0) >> 4;
    switch (highnib) {
    case 0x00:
      break;
    case 0x01: {
      // JUMP $NNN
      // Jump to NNN
      uint16_t target = ((op[0] & 0xf) << 8) | op[1];
      m_PC = target;
    } break;
    case 0x02: {
      // CALL $NNN
      // Call subroutine at NNN

      // Advance stack pointer
      m_SP = -2;

      // Store next instructions address to memory pointed by the stack pointer
      m_memory[m_SP] = ((m_PC + 2) & 0xff00) >> 8;
      m_memory[m_SP + 1] = ((m_PC + 2) & 0x00ff);

      // Jump to subroutines address NNN
      uint16_t target = ((op[0] & 0xf) << 8) | op[1];
      m_PC = target;
    } break;
    case 0x03: {
      uint8_t reg = op[0] & 0x0f;
      if (m_V[reg] == op[1])
        m_PC += 2;
      m_PC += 2;
    } break;
    case 0x04: {
      uint8_t reg = op[0] & 0x0f;
      if (m_V[reg] != op[1])
        m_PC += 2;
      m_PC += 2;
    } break;
    case 0x05: {
      uint8_t reg1 = op[0] & 0x0f;
      uint8_t reg2 = (op[1] & 0xf0) >> 4;
      if (m_V[reg1] == m_V[reg2])
        m_PC += 2;
      m_PC += 2;
    } break;
    case 0x06: {
      uint8_t reg = op[0] & 0x0f;
      m_V[reg] = op[1];
      m_PC += 2;
    } break;
    case 0x07: {
      uint8_t reg = op[0] & 0x0f;
      m_V[reg] += op[1];
      m_PC += 2;
    } break;
    case 0x08: {
      uint8_t command = op[1] & 0x0f;
      uint8_t reg1 = op[0] & 0x0f;
      uint8_t reg2 = (op[1] & 0xf0) >> 4;

      switch (command) {
      case 0x1: {
        m_V[reg1] |= m_V[reg2];
      } break;
      case 0x2: {
        m_V[reg1] &= m_V[reg2];
      } break;
      case 0x3: {
        m_V[reg1] ^= m_V[reg2];
      } break;
      case 0x4: {
        uint16_t result = m_V[reg1] + m_V[reg2];
        m_V[0xf] = (result & 0xff00) ? 1 : 0;
        m_V[reg1] = result & 0xff;
      } break;
      case 0x5: {
        uint8_t result = m_V[reg1] - m_V[reg2];
        m_V[0xf] = (result > 0) ? 1 : 0;
        m_V[reg1] = result;
      } break;
      case 0x6: {
        m_V[0xf] = m_V[reg1] & 0x1;
        m_V[reg1] = m_V[reg1] >> 1;
      } break;
      case 0x7: {
        uint8_t result = m_V[reg2] - m_V[reg1];
        m_V[0xf] = (result > 0) ? 1 : 0;
        m_V[reg1] = result;
      } break;
      case 0xe: {
        m_V[0xf] = (0x80 == (m_V[reg1] & 0x80));
        m_V[reg1] = m_V[reg1] << 1;
      } break;
      }
      m_PC += 2;
    } break;
    case 0x09: {
      uint8_t reg1 = op[0] & 0xf;
      uint8_t reg2 = (op[1] & 0xf0) >> 4;
      if (m_V[reg1] != m_V[reg2])
        m_PC += 2;
      m_PC += 2;
    }
    case 0x0a: {
      m_I = ((op[0] & 0xf) << 8) | op[1];
      m_PC += 2;
    } break;
    case 0x0b: {
      uint16_t num = ((op[0] & 0xf) << 8) | op[1];
      m_PC = m_V[0] + num;
    } break;
    case 0x0c: {
      uint8_t reg = op[0] & 0xf;
      uint8_t num = op[1];
      m_V[reg] = (rand() % 256) + num;
      m_PC += 2;
    } break;
    case 0x0d: {
      // TODO: Drawing sprites
    } break;
    case 0x0e: {
      int reg = op[0] & 0xf;
      switch (op[1]) {
      case 0x9e: {
        // Ex9E - SKP Vx
        // Skip next instruction if key stored in Vx is pressed
        uint8_t key = m_V[reg];
        // Add 2 to program counter to skip next instruction
        if (m_keyboard.isPressed(key))
          m_PC += 2;
      } break;
      case 0xa1: {
        // ExA1 - SKMP Vx
        // Skip next instruction if key stored in Vx is not pressed
        uint8_t key = m_V[reg];
        // Add 2 to program counter to skip next instruction
        if (!m_keyboard.isPressed(key))
          m_PC += 2;
      } break;
      }
    } break;
    case 0x0f: {
      int reg = op[0] & 0xf;
      switch (op[1]) {
      case 0x07: {
        m_V[reg] = m_delay;
        m_PC += 2;
      } break;
      case 0x0A: {
        // Fx0A - LD Vx, K
        // Wait for key press, store value of the key in Vx
        uint8_t key = m_keyboard.waitKeyPress();
        m_V[reg] = key;
      } break;
      case 0x15: {
        m_delay = m_V[reg];
      } break;
      case 0x18: {
        m_sound = m_V[reg];
      } break;
      case 0x1e: {
        m_I = m_V[reg] + m_I;
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
        uint8_t value = m_V[reg];
        ones = value % 10;
        value /= 10;
        tens = value % 10;
        hundreds = value / 10;
        m_memory[m_I] = hundreds;
        m_memory[m_I + 1] = tens;
        m_memory[m_I + 2] = ones;
      } break;
      case 0x55: {
        /* Fx55 - LD [I], Vx
         * Store registers V0 to Vx in memory starting at location I.
         */
        for (auto i = 0; i < reg; i++)
          m_memory[m_I + i] = m_V[reg + i];
      } break;
      case 0x65: {
        /* Fx65 - LD Vx, [I]
         * Read registers V0 to Vx from memory starting at location I.
         */
        for (auto i = 0; i < reg; i++)
          m_V[reg + i] = m_memory[m_I + i];
      } break;
      }
      m_PC += 2;
    } break;
    }
  }

private:
  uint8_t m_V[16]; // Registers 0-F
  uint16_t m_I;    // Index register
  uint16_t m_SP;   // Stack pointer
  uint16_t m_PC;   // Program counter
  uint8_t m_delay; // Delay timer
  uint8_t m_sound; // Sound timer
  uint8_t *m_memory;
  uint8_t *m_screen; // Same as memory[0xF00]

  bool m_ready = false;

  Display m_display;
  Keyboard m_keyboard;
};

int main(int argc, char **argv) {
  Chip8 vm;
  vm.init();
  vm.load_rom(argv[1]);

  bool quitting = false;
  while (!quitting) {
    vm.emulate();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        quitting = true;
    }

    SDL_Delay(2);
  }

  return 0;
}
