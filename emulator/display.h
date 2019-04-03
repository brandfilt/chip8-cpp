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

    SDL_CreateWindowAndRenderer(640, 320, SDL_WINDOW_OPENGL, &m_window,
                                &m_renderer);
    SDL_RenderSetLogicalSize(m_renderer, 64, 32);

    if (m_window == nullptr)
      return 1;

    m_glcontext = SDL_GL_CreateContext(m_window);
    return 0;
  }

  void update(uint8_t *screen) {
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
    SDL_RenderClear(m_renderer);
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    for (auto i = 0; i < 256; i++) {
      uint8_t screenByte = screen[i];
      for (auto a = 0; a < 8; a++) {
        int bit = i * 8 + a;
        int y = bit / 64;
        int x = bit - y;
        if (screenByte & 0x01)
          SDL_RenderDrawPoint(m_renderer, x, y);

        screenByte = screenByte >> 1;
      }
    }

    SDL_RenderPresent(m_renderer);
  }

private:
  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;
  SDL_GLContext m_glcontext;
};

#endif
