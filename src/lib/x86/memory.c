#include <stdint.h>
#include "../memory.h"

uint_least8_t spectrum48Memory[0xFFFF];

uint_least8_t memRead(uint_least16_t address) {
	return spectrum48Memory[address];
}

void memWrite(uint_least16_t address, uint_least8_t data) {
	spectrum48Memory[address] = data;
}
