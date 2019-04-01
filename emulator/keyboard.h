#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <SDL.h>

/* Keyboard contains simple utility methods for getting keyboard input */
class Keyboard {
public:
  Keyboard() {}
  bool isPressed(const uint8_t &key) const {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN) {
        uint8_t keysym = event.key.keysym.sym;

        if (keysym == key)
          return true;
      }
    }

    return false;
  }
  void waitKeyPress(const uint8_t &key) const {}
};

#endif KEYBOARD_H
