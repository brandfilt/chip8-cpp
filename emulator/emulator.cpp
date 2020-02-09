#include <bitset>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>

#include <SDL.h>

#include "display.h"
#include "keyboard.h"

#include <nlohmann/json.hpp>

#include "../disassembler/disassembler.h"

#include "timer.h"

const uint16_t CLOCK_SPEED_HZ = 500;

using json = nlohmann::json;

template<typename T>
std::string int_to_hex(T i) {
  std::stringstream stream;
  stream << std::uppercase << std::hex << i;
  return stream.str();
}


class Chip8 {
public:
  Chip8()
      : m_delay(60), m_sound(60), m_clock_speed(CLOCK_SPEED_HZ),
        m_step_mode(false), m_step(false) {
    m_memory = new unsigned char[1024 * 4 + 0x200]; // 4K memory reserved
    m_screen = &m_memory[0xF00];
    m_I = 0x00;
    m_SP = 0x70;
    m_PC = 0x200; // Programs are loaded at 0x200

    const unsigned char fontset[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for (auto i = 0; i < (1024 * 4 + 0x200); i++) {
      m_memory[i] = 0;
    }

    for (auto i = 0; i < 80; i++) {
      m_memory[i] = fontset[i];
    }
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

  void run() {
    while (!m_quitting) {
      emulate();
      m_display.update(m_screen);
      m_keyboard.pollEvents();

      if (m_keyboard.keyDownEvent(SDLK_SPACE)) {
        m_step = true;
      }

      if (m_keyboard.keyDownEvent(SDLK_p)) {
        m_step_mode = !m_step_mode;
      }

      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
          m_quitting = true;
      }
      SDL_Delay(1000 / m_clock_speed);
    }

    print_debug();

  }

  void init() { m_display.init(); }

  void print_debug() {

    json debug = {{"I", static_cast<int>(m_I)},
                  {"PC", static_cast<int>(m_PC)},
                  {"SP", static_cast<int>(m_SP)}};

    for (auto i = 0; i < 16; i++) {
      debug.emplace("V" + int_to_hex(i), static_cast<int>(m_V[i]));
    }

    std::cout << debug << std::endl;
  }

  void emulate() {
    if (!m_ready)
      return;

    if (m_step_mode && !m_step)
      return;

    // Fetch first 8 bits of next instruction
    uint8_t *op = &m_memory[m_PC];

    uint8_t firstbyte = op[0];
    uint8_t lastbyte = op[1];

    int highnib = (*op & 0xf0) >> 4;
    switch (highnib) {
    case 0x00:
      switch (lastbyte) {
      case 0xFD:
        // EXIT
        // Exits the program
        m_quitting = true;
        // Don't advance PC on exit.
        m_PC -= 2;
        break;
      case 0xE0: {
        // CLS
        // Clear the display
        int byte_count = SCREEN_WIDTH * SCREEN_HEIGHT / 8;
        for (auto i = 0; i < byte_count; i++) {
          m_screen[i] = 0;
        }
        break;
      }
      case 0xEE:
        // RET
        // Return from subroutine
        m_PC = m_memory[m_SP] << 8 | m_memory[m_SP + 1];
        m_SP += 2;
        return;
      }
      m_PC += 2;
      break;
    case 0x01: {
      // JUMP 1NNN
      // Jump to NNN
      uint16_t target = ((firstbyte & 0x0f) << 8) | lastbyte;
      m_PC = target;
    } break;
    case 0x02: {
      // CALL 2NNN
      // Call subroutine at NNN

      // Advance stack pointer
      m_SP -= 2;

      // Store next instructions address to memory pointed by the stack
      // pointer
      m_memory[m_SP] = ((m_PC + 2) & 0xff00) >> 8;
      m_memory[m_SP + 1] = ((m_PC + 2) & 0x00ff);

      // Jump to subroutines address NNN
      uint16_t target = ((firstbyte & 0xf) << 8) | lastbyte;
      m_PC = target;
    } break;
    case 0x03: {
      // 3xkk SE Vx, byte
      // Skip next instructions if Vx = kk
      uint8_t reg = firstbyte & 0x0f;
      if (m_V[reg] == lastbyte)
        m_PC += 2;
      m_PC += 2;
    } break;
    case 0x04: {
      // 4xkk SNE Vx, byte
      // Skip next instruction if Vx != kk
      uint8_t reg = firstbyte & 0x0f;
      if (m_V[reg] != lastbyte)
        m_PC += 2;
      m_PC += 2;
    } break;
    case 0x05: {
      // 5xy0 SE Vx, Vy
      // Skip next instruction if Vx = Vy
      uint8_t reg1 = firstbyte & 0x0f;
      uint8_t reg2 = (lastbyte & 0xf0) >> 4;
      if (m_V[reg1] == m_V[reg2])
        m_PC += 2;
      m_PC += 2;
    } break;
    case 0x06: {
      // 6xkk LD Vx, byte
      // Set Vx = kk
      uint8_t reg = firstbyte & 0x0f;
      m_V[reg] = lastbyte;
      m_PC += 2;
    } break;
    case 0x07: {
      // 7xkk ADD Vx, byte
      // Set Vx = Vx + kk
      uint8_t reg = firstbyte & 0x0f;
      m_V[reg] += lastbyte;
      m_PC += 2;
    } break;
    case 0x08: {
      uint8_t command = lastbyte & 0x0f;
      uint8_t reg1 = firstbyte & 0x0f;
      uint8_t reg2 = (lastbyte & 0xf0) >> 4;

      switch (command) {
      case 0x0: {
        // 8xy0 LD Vx, Vy
        // Set Vx = Vy
        m_V[reg1] = m_V[reg2];
      }
      case 0x1: {
        // 8xy1 OR Vx, Vy
        // Bitwise OR on Vx and Vy. The result is stored to Vx.
        m_V[reg1] |= m_V[reg2];
      } break;
      case 0x2: {
        // 8xy1 AND Vx, Vy
        // Bitwise AND on Vx and Vy. The result is stored to Vx.
        m_V[reg1] &= m_V[reg2];
      } break;
      case 0x3: {
        // 8xy1 XOR Vx, Vy
        // Bitwise XOR on Vx and Vy. The result is stored to Vx.
        m_V[reg1] ^= m_V[reg2];
      } break;
      case 0x4: {
        // 8xy4 ADD Vx, Vy
        // Set Vx = Vx + Vy, set VF = carry
        //  Values of Vx and Vy are added together.
        // If the result is > 255, VF is set to 1.
        uint16_t result = m_V[reg1] + m_V[reg2];
        m_V[0xf] = (result > 0xff) ? 1 : 0;
        m_V[reg1] = result & 0xff;
      } break;
      case 0x5: {
        // 8xy5 SUB Vx, Vy
        // Set Vx = Vx - Vy, set VF = NOT borrow
        // If Vx > Vy, VF is set to 1.
        uint8_t vx = m_V[reg1];
        uint8_t vy = m_V[reg2];
        uint8_t result = vx - vy;
        m_V[0xf] = (vx > vy) ? 1 : 0;
        m_V[reg1] = result;
      } break;
      case 0x6: {
        // 8xy6 SHR Vx {, Vy}
        // Set Vx = Vx SHR 1
        // If the least significant bit of Vx is 1, set VF to 1.
        // Divide Vx by 2.
        m_V[0xf] = m_V[reg1] & 0x1;
        m_V[reg1] = m_V[reg1] >> 1;
      } break;
      case 0x7: {
        // 8xy7 SUBN Vx, Vy
        // Set Vx = Vy - Vx, set VF = NOT borrow
        // If Vy > Vx, set VF 1.
        uint8_t vx = m_V[reg1];
        uint8_t vy = m_V[reg2];
        uint8_t result = vy - vx;
        m_V[0xf] = (vx < vy) ? 1 : 0;
        m_V[reg1] = result;
      } break;
      case 0xe: {
        // 8xyE SHL Vx {, Vy}
        // Set Vx = Vx SHL 1
        // If the most significant bit of Vx is 1, set VF to 1.
        // Multiply Vx by 2;
        m_V[0xf] = (m_V[reg1] & 0x8) >> 7;
        m_V[reg1] = m_V[reg1] << 1;
      } break;
      }
      m_PC += 2;
    } break;
    case 0x09: {
      // 9xy0 SNE Vx, Vy
      // Skip next instruction if Vx != Vy
      uint8_t reg1 = firstbyte & 0xf;
      uint8_t reg2 = (lastbyte & 0xf0) >> 4;
      if (m_V[reg1] != m_V[reg2])
        m_PC += 2;
      m_PC += 2;
    }
    case 0x0a: {
      // Annn LD I, addr
      // The value of the register I is set to nnn.
      m_I = ((firstbyte & 0xf) << 8) | lastbyte;
      m_PC += 2;
    } break;
    case 0x0b: {
      // Bnnn JP V0, addr
      // Jump to location nnn + V0
      uint16_t num = ((firstbyte & 0xf) << 8) | lastbyte;
      m_PC = m_V[0] + num;
    } break;
    case 0x0c: {
      // Cxkk RND Vx, byte
      // Set Vx = random byte AND kk
      uint8_t reg = firstbyte & 0xf;
      uint8_t num = lastbyte;
      m_V[reg] = (rand() % 256) & num;
      m_PC += 2;
    } break;
    case 0x0d: {
      // Dxyn DRW Vx, Vy, nibble
      //   Display n-byte sprite starting at memory location I at (Vx, Vy),
      //   set VF = collision.

      //   The interpreter reads n bytes from memory, starting at the address
      //   stored in I. These bytes are then displayed as sprites on screen at
      //   coordinates (Vx, Vy). Sprites are XORed onto the existing screen.
      //   If this causes any pixels to be erased, VF is set to 1, otherwise
      //   it is set to 0. If the sprite is positioned so part of it is
      //   outside the coordinates of the display, it wraps around to the
      //   opposite side of the screen. See instruction 8xy3 for more
      //   information on XOR, and section 2.4, Display, for more information
      //   on the Chip-8 screen and sprites.
      uint8_t x_reg = firstbyte & 0x0f;
      uint8_t y_reg = (lastbyte >> 4) & 0x0f;
      uint8_t n = lastbyte & 0x0f;

      uint8_t x = m_V[x_reg];
      uint8_t y = m_V[y_reg];

      int bit_position = y * SCREEN_WIDTH + x;
      int bit_offset = bit_position % 8;
      int byte_position = (bit_position - bit_offset) / 8;
      int overflow_bit_position = y * SCREEN_WIDTH + x + 8;
      int overflow_bit_offset = overflow_bit_position % 8;
      int overflow_byte_position =
          (overflow_bit_position - overflow_bit_offset) / 8;

      bool erased = false;
      for (auto i = 0; i < n; i++) {

        int screen_byte_position = byte_position + i * SCREEN_WIDTH / 8;
        uint8_t screen_byte = m_screen[screen_byte_position];
        if ((screen_byte >> bit_offset) > 0) erased = true;

        uint8_t byte = m_memory[m_I + i];
        m_screen[screen_byte_position] ^= (byte >> bit_offset);

        if (overflow_bit_offset > 0) {
          m_screen[overflow_byte_position + i * SCREEN_WIDTH / 8] ^=
              (byte << (8 - overflow_bit_offset));
        }

        if (i == n) {
          m_screen[byte_position + i + 1] ^= (byte << (8 - bit_offset));
        }
      }

      m_V[0xf] = erased ? 1 : 0;

      m_PC += 2;
    } break;
    case 0x0e: {
      int reg = firstbyte & 0xf;
      switch (lastbyte) {
      case 0x9e: {
        // Ex9E SKP Vx
        // Skip next instruction if key stored in Vx is pressed
        uint8_t key = m_V[reg];
        // Add 2 to program counter to skip next instruction
        if (m_keyboard.isPressed(key))
          m_PC += 2;
        m_PC += 2;
      } break;
      case 0xa1: {
        // ExA1 SKNP Vx
        // Skip next instruction if key stored in Vx is not pressed
        uint8_t key = m_V[reg];
        // Add 2 to program counter to skip next instruction
        if (!m_keyboard.isPressed(key))
          m_PC += 2;
        m_PC += 2;
      } break;
      }
    } break;
    case 0x0f: {
      int reg = firstbyte & 0x0f;
      switch (lastbyte) {
      case 0x07: {
        // Fx07 LD Vx, DT
        // Set Vx = the delay timer
        m_V[reg] = m_delay.value();
        // m_PC += 2;
      } break;
      case 0x0A: {
        // Fx0A LD Vx, K
        // Wait for key press, store value of the key in Vx
        // m_V[reg] = m_keyboard.waitKeyPress();
        if (!m_keyboard.anyKeyDownEvents()) {
          m_PC -= 2;
        } else {
          uint8_t key = m_keyboard.lastPressed();
          m_V[reg] = key;
        }
      } break;
      case 0x15: {
        // Fx15 LD DT, Vx
        // Set delay timer = Vx
        m_delay.setValue(m_V[reg]);
      } break;
      case 0x18: {
        // Fx18 LD ST, Vx
        // Set sound timer = Vx
        m_sound = m_V[reg];
      } break;
      case 0x1e: {
        // Fx1E ADD I, Vx
        // Set I = I + Vx
        m_I = m_V[reg] + m_I;
      } break;
      case 0x29: {
        // Fx29 - LD F, Vx
        // Set I to location of the sprite for digit stored in Vx
        m_I = 5 * m_V[reg];
      } break;
      case 0x33: {
        // Fx33 - LD B, Vx
        // Store Binary Coded Decimal representation of Vx in memory
        // locations I, I+1 and I+2.
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
        // Fx55 - LD [I], Vx
        // Store registers V0 to Vx in memory starting at location I.
        for (auto i = 0; i <= reg; i++)
          m_memory[m_I + i] = m_V[i];
      } break;
      case 0x65: {
        // Fx65 - LD Vx, [I]
        // Read registers V0 to Vx from memory starting at location I.
        for (auto i = 0; i <= reg; i++)
          m_V[i] = m_memory[m_I + i];
      } break;
      }
      m_PC += 2;
    } break;
    }

    if (m_step_mode)
      m_step = false;

    m_delay.update(m_clock_speed);
    m_sound.update(m_clock_speed);
  }

private:
  uint8_t m_V[16]; // Registers 0-F
  uint16_t m_I;    // Index register
  uint16_t m_SP;   // Stack pointer
  uint16_t m_PC;   // Program counter
  Timer m_delay;   // Delay timer
  Timer m_sound;   // Sound timer
  uint8_t *m_memory;
  uint8_t *m_screen; // Same as memory[0xF00]

  uint16_t m_clock_speed;
  bool m_step_mode;
  bool m_step;

  bool m_ready = false;
  bool m_quitting = false;

  Display m_display;
  Keyboard m_keyboard;
};

int main(int argc, char **argv) {
  Chip8 vm;
  vm.init();
  vm.load_rom(argv[1]);
  vm.run();

  return 0;
}
