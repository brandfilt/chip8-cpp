#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <SDL.h>

/* Keyboard contains simple utility methods for getting keyboard input */
class Keyboard {
 public:
  Keyboard() {}
  bool isPressed(const uint8_t &key) const {}
  void waitKeyPress(const uint8_t &key) const {}
};

#endif KEYBOARD_H
