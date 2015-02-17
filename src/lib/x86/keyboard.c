#include "../keyboard.h"
#include "SDL2/SDL.h"

// unsigned char * keys = SDL_GetKeyState(NULL);

uint_least8_t getKeyPress(uint_least8_t row) {
	SDL_PumpEvents();
	const uint_least8_t *keys = SDL_GetKeyboardState(NULL);
	switch(row) {
		case 0xFE: // CAPS/SHIFT - V
			if(keys[SDL_SCANCODE_LSHIFT]) return 0xFE;
			if(keys[SDL_SCANCODE_Z]) return 0xFD;
			if(keys[SDL_SCANCODE_X]) return 0xFB;
			if(keys[SDL_SCANCODE_C]) return 0xF7;
			if(keys[SDL_SCANCODE_V]) return 0xEF;
			return 0xBF;
		case 0xFD: // A - G
			if(keys[SDL_SCANCODE_A]) return 0xFE;
			if(keys[SDL_SCANCODE_S]) return 0xFD;
			if(keys[SDL_SCANCODE_D]) return 0xFB;
			if(keys[SDL_SCANCODE_F]) return 0xF7;
			if(keys[SDL_SCANCODE_G]) return 0xEF;
			return 0xBF;
		case 0xFB: // Q - T
			if(keys[SDL_SCANCODE_Q]) return 0xFE;
			if(keys[SDL_SCANCODE_W]) return 0xFD;
			if(keys[SDL_SCANCODE_E]) return 0xFB;
			if(keys[SDL_SCANCODE_R]) return 0xF7;
			if(keys[SDL_SCANCODE_T]) return 0xEF;
			return 0xBF;
		case 0xF7: // 1 - 5
			if(keys[SDL_SCANCODE_1]) return 0xFE;
			if(keys[SDL_SCANCODE_2]) return 0xFD;
			if(keys[SDL_SCANCODE_3]) return 0xFB;
			if(keys[SDL_SCANCODE_4]) return 0xF7;
			if(keys[SDL_SCANCODE_5]) return 0xEF;
			return 0xBF;
		case 0xEF: // 6 - 0
			if(keys[SDL_SCANCODE_0]) return 0xFE;
			if(keys[SDL_SCANCODE_9]) return 0xFD;
			if(keys[SDL_SCANCODE_8]) return 0xFB;
			if(keys[SDL_SCANCODE_7]) return 0xF7;
			if(keys[SDL_SCANCODE_6]) return 0xEF;
			return 0xBF;
		case 0xDF: // Y - P
			if(keys[SDL_SCANCODE_P]) return 0xFE;
			if(keys[SDL_SCANCODE_O]) return 0xFD;
			if(keys[SDL_SCANCODE_I]) return 0xFB;
			if(keys[SDL_SCANCODE_U]) return 0xF7;
			if(keys[SDL_SCANCODE_Y]) return 0xEF;
			return 0xBF;
		case 0xBF: // H - ENTER
			if(keys[SDL_SCANCODE_1]) return 0xFE;
			if(keys[SDL_SCANCODE_L]) return 0xFD;
			if(keys[SDL_SCANCODE_K]) return 0xFB;
			if(keys[SDL_SCANCODE_J]) return 0xF7;
			if(keys[SDL_SCANCODE_H]) return 0xEF;
			return 0xBF;
		case 0x7F: // B - SPACE
			if(keys[SDL_SCANCODE_SPACE]) return 0xFE;
			if(keys[SDL_SCANCODE_RSHIFT]) return 0xFD;
			if(keys[SDL_SCANCODE_M]) return 0xFB;
			if(keys[SDL_SCANCODE_N]) return 0xF7;
			if(keys[SDL_SCANCODE_B]) return 0xEF;
			return 0xBF;
		default:
			return 0xBF;
	}
}