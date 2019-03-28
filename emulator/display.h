#ifndef DISPLAY_H
#define DISPLAY_H


/* Display handles drawing the Chip8 screen contents.
 * The actual screen data is stored in emulator memory
 * and it's passed to the update method.
 */
class Display {
 public:
  void update(const uint8_t &screen);
  void clear(uint8_t *screen);

}

#endif
