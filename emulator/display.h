#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL.h>
#include <SDL_opengl.h>

const uint8_t SCREEN_WIDTH = 64;
const uint8_t SCREEN_HEIGHT = 32;

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
    int byte_count = SCREEN_WIDTH * SCREEN_HEIGHT / 8;
    for (auto i = 0; i < byte_count; i++) {
      uint8_t screen_byte = screen[i];
      for (auto a = 7; a >= 0; a--) {
        int bit = i * 8 + a;
        int y = bit / SCREEN_WIDTH;
        int x = bit - y * SCREEN_WIDTH;
        if (screen_byte & 0x01) {
          SDL_RenderDrawPoint(m_renderer, x, y);
          // std::cout << x << ", " << y << std::endl;
        }

        screen_byte = screen_byte >> 1;
      }
    }

    SDL_RenderPresent(m_renderer);
  }

  void print_debug(uint8_t *screen) {
    int byte_count = SCREEN_WIDTH * SCREEN_HEIGHT / 8;
    std::cout << "SCREEN START" << std::endl;
    for (auto i = 0; i < byte_count; i++) {
      uint8_t screen_byte = screen[i];
      if ((i*8) % SCREEN_WIDTH == 0) {
        std::cout << std::endl;
      }

      std::cout << std::hex << std::setfill('0')
                << std::setw(2) << static_cast<int>(screen_byte) << " ";
    }
    std::cout << "SCREEN END" << std::endl;
  }

private:
  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;
  SDL_GLContext m_glcontext;
};

#endif
