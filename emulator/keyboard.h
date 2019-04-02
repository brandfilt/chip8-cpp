#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <SDL.h>
#include <set>

/* Keyboard contains simple utility methods for getting keyboard input */
class Keyboard {
public:
  Keyboard() {}
  bool isPressed(const uint8_t &key) const { return m_pressed.count(key) == 1; }

  uint8_t waitKeyPress() const {
    SDL_Event event;
    while (true) {
      SDL_PollEvent(&event);
      if (event.type == SDL_KEYDOWN) {
        return event.key.keysym.sym;
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