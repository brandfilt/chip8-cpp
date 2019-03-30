#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL.h>
#include <SDL_opengl.h>

/* Display handles drawing the Chip8 screen contents.
 * The actual screen data is stored in emulator memory
 * and it's passed to the update method.
 */
class Display {
public:
  Display() {}
  ~Display() {
    SDL_Log("DESTROY\n");
    SDL_GL_DeleteContext(m_glcontext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
  }

  /* Initializes SDL and creates a window and OpenGL context for rendering */
  int init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
      return 1;
    }

    m_window =
        SDL_CreateWindow("Chip8", 0,
                         0, 640, 320, SDL_WINDOW_SHOWN);

    if (m_window == nullptr)
      return 1;

    m_glcontext = SDL_GL_CreateContext(m_window);
    return 0;
  }

  void update(const uint8_t &screen) {}

private:
  SDL_Window *m_window = nullptr;
  SDL_GLContext m_glcontext;
};

#endif
