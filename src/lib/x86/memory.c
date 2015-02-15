#include <stdint.h>
#include "../memory.h"

static uint_least8_t spectrum48Memory[0xFFFF] = { 0x00 };

uint_least8_t memRead(uint_least16_t address) {
	return spectrum48Memory[address];
}

void memWrite(uint_least16_t address, uint_least8_t data) {
	spectrum48Memory[address] = data;
}
