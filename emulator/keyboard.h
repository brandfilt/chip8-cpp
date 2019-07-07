#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <SDL.h>
#include <set>

/* Keyboard contains simple utility methods for getting keyboard input */
class Keyboard {
public:
  Keyboard() {}
  bool isAnyPressed() const { return m_pressed.size() > 0; }
  bool isPressed(const uint8_t &key) const { return m_pressed.count(key) == 1; }
  uint8_t lastPressed() const {return *m_pressed.rbegin();}

  void pollEvents() {
    // m_pressed.clear();
    for (auto key : m_pressed) 
      m_keyDown.erase(key);

    SDL_Event event;
    SDL_PollEvent(&event);
    if (event.type == SDL_KEYDOWN) {
      uint8_t keysym = event.key.keysym.sym;
      uint8_t key = Keyboard::keySymToChip8Key(keysym);
      if (m_pressed.count(key) == 0) {
        m_pressed.insert(key);
        m_keyDown.insert(key);
      }

    } else if (event.type == SDL_KEYUP) {
      uint8_t keysym = event.key.keysym.sym;
      m_pressed.erase(Keyboard::keySymToChip8Key(keysym));
    }
  }


  bool keyDownEvent(const uint8_t &key) const { return m_keyDown.count(key) == 1; }
  bool anyKeyDownEvents() const {return m_keyDown.size() > 0;}


private:
  std::set<uint8_t> m_pressed;
  std::set<uint8_t> m_keyDown;

  static uint8_t keySymToChip8Key(uint8_t sym) {
    switch(sym) {
    case SDLK_0:
      return 0x00;
    case SDLK_1:
      return 0x01;
    case SDLK_2:
      return 0x02;
    case SDLK_3:
      return 0x03;
    case SDLK_4:
      return 0x04;
    case SDLK_5:
      return 0x05;
    case SDLK_6:
      return 0x06;
    case SDLK_7:
      return 0x07;
    case SDLK_8:
      return 0x08;
    case SDLK_9:
      return 0x09;
    case SDLK_a:
      return 0x0A;
    case SDLK_b:
      return 0x0B;
    case SDLK_c:
      return 0x0C;
    case SDLK_d:
      return 0x0D;
    case SDLK_e:
      return 0x0E;
    case SDLK_f:
      return 0x0F;
    default:
      return 0xFF;
    }
  }
};

#endif
