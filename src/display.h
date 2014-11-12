#include "SDL2/SDL.h"
#include "z80.h"

void initDisplay(u8 memory[]);
void draw(u8 memory[], SDL_Surface* surface);
int generateColor(u8 attributeByte, u8 isForeground);