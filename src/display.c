#include "SDL2/SDL.h"
#include "display.h"

// ZX Spectrum resolution.
const int WIDTH = 256;
const int HEIGHT = 192;

void initDisplay() {
  SDL_Window* window;
  SDL_Surface* surface;

  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("SEEM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  surface = SDL_GetWindowSurface(window);

  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
  SDL_UpdateWindowSurface(window);
  SDL_Delay(2000);
}