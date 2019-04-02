#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <SDL.h>
#include <set>

/* Keyboard contains simple utility methods for getting keyboard input */
class Keyboard {
public:
  Keyboard() {}
  bool isPressed(const uint8_t &key) const { return m_pressed.count(key) == 1; }

  void waitKeyPress(const uint8_t &key) const {
    SDL_Event event;
    while (true) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) {
          uint8_t keysym = event.key.keysym.sym;
          if (keysym == key)
            return;
        }
      }
    }
  }

  void pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN) {
        uint8_t keysym = event.key.keysym.sym;
        m_pressed.insert(keysym);
      } else if (event.type == SDL_KEYUP) {
        uint8_t keysym = event.key.keysym.sym;
        m_pressed.erase(keysym);
      }
    }
  }

private:
  std::set<uint8_t> m_pressed;
};

#endif KEYBOARD_H
