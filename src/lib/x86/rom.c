#include "../rom.h"
#include "../memory.h"

// void loadZ80Snapshot();
// void load48KRom();

void load48KRom() {
	int romLength = sizeof(rom48K) / sizeof(rom48K[0]);
	int romIndex;

	for(romIndex = 0x0; romIndex < romLength; romIndex++) {
		memWrite(romIndex, rom48K[romIndex]);
	}
}