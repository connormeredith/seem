#include "SDL2/SDL.h"
#include "z80.h"

void initDisplay(u8 memory[]);
void render(u8 memory[], SDL_Window* window);
int pixelColor(u8 attributeByte, u8 isForeground);