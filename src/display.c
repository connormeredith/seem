#include "SDL2/SDL.h"
#include "display.h"
#include "z80.h"

// ZX Spectrum resolution.
const int WIDTH = 256;
const int HEIGHT = 192;

void initDisplay(u8 memory[]) {
  SDL_Window* window;
  SDL_Surface* surface;

  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("SEEM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  surface = SDL_GetWindowSurface(window);

  draw(memory, surface);

  SDL_UpdateWindowSurface(window);
  SDL_Delay(10000);
}

void draw(u8 memory[], SDL_Surface* surface) {
	unsigned int *ptr = (unsigned int*)surface->pixels;
	u8 actualRow, displayByte, attributeByte, currentPixelBit;
	int rowPointer;
	u8 offset = 0;

	for(int vCount = 0; vCount < 192; vCount++) {
		if((vCount % 8) == 0 && vCount != 0) offset++;
		if((vCount % 64) == 0 && vCount != 0) offset = (offset - 8) + 64;

		actualRow = (((vCount % 8) * 8) + offset);
		rowPointer = actualRow * (surface->pitch / sizeof(unsigned int));

		for(int hCount = 0; hCount < 256; hCount++) {
			displayByte = memory[0x4000 + (hCount >> 3) + (vCount << 5)];
			attributeByte = memory[0x5800 + ((actualRow >> 3) << 5) + (hCount >> 3)];
			currentPixelBit = 7 - (hCount % 8);

			ptr[rowPointer + hCount] = generateColor(attributeByte, (displayByte & (1 << currentPixelBit)));
		}
	}
}

int generateColor(u8 attributeByte, u8 isForeground) {
	int color = 0x0;

	if(isForeground) {
		if(attributeByte & (1 << 0)) {
			color += 0xFF;
		}
		if(attributeByte & (1 << 1)) {
			color += 0xFF0000;
		}
		if(attributeByte & (1 << 2)) {
			color += 0xFF00;
		}
	} else {
		if(attributeByte & (1 << 3)) {
			color += 0xFF;
		}
		if(attributeByte & (1 << 4)) {
			color += 0xFF0000;
		}
		if(attributeByte & (1 << 5)) {
			color += 0xFF00;
		}
	}

	return color;
}