#include <stdint.h>

#include "SDL2/SDL.h"
#include "../display.h"
#include "../memory.h"

// Private functions.
static int pixelColor(uint_least8_t attributeByte, uint_least8_t isForeground);

// ZX Spectrum resolution.
const int WIDTH = 256;
const int HEIGHT = 192;

// Screen vars.
SDL_Window* window;
SDL_Event event;

/**
 * Initialises the display window.
 * @param memory The ZX Spectrum's memory array.
 */
SDL_Event initDisplay() {
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("SEEM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

  render();
  return event;
}

/**
 * Renders the screen from video memory (0x4000 - 0x51AA).
 * @param memory  The ZX Spectrum's memory array.
 * @param surface A pointer to the window's surface struct.
 */
void render() {
	SDL_Surface* surface = SDL_GetWindowSurface(window);
	SDL_LockSurface(surface);

	unsigned int *ptr = (unsigned int*)surface->pixels;

	uint_least8_t actualRow, displayByte, attributeByte, currentPixelBit;
	int rowPointer;
	uint_least8_t offset = 0;
	int vCount;

	for(vCount = 0; vCount < 192; vCount++) {
		int hCount;

		if((vCount % 8) == 0 && vCount != 0) offset++;
		if((vCount % 64) == 0 && vCount != 0) offset = (offset - 8) + 64;

		actualRow = (((vCount % 8) * 8) + offset);
		rowPointer = actualRow * (surface->pitch / sizeof(unsigned int));

		for(hCount = 0; hCount < 256; hCount++) {
			displayByte = memRead(0x4000 + (hCount >> 3) + (vCount << 5));
			attributeByte = memRead(0x5800 + ((actualRow >> 3) << 5) + (hCount >> 3));
			currentPixelBit = 7 - (hCount % 8);

			ptr[rowPointer + hCount] = pixelColor(attributeByte, (displayByte & (1 << currentPixelBit)));
		}
	}

	SDL_UnlockSurface(surface);
	SDL_UpdateWindowSurface(window);
}

/**
 * Returns the color of an individual pixel.
 * @param  attributeByte The attribute for the pixel's character block.
 * @param  isForeground  Whether or not the pixel is to use the foreground or background color.
 * @return               The color of the pixel in hex.
 */
static int pixelColor(uint_least8_t attributeByte, uint_least8_t isForeground) {
	// Converts the 3 color bits of an attribute byte into hex depending on their value.
	static int spectrumColor[8] = { 0x00, 0xFF, 0xFF0000, 0xFF00FF, 0xFF00, 0xFFFF, 0xFFFF00, 0xFFFFFF };

	uint_least8_t colorByte = (isForeground) ? (attributeByte & 0x7) : ((attributeByte & 0x38) >> 3);
	return spectrumColor[colorByte];
}
