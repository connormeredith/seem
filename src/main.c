#include <stdlib.h>
#include <stdio.h>

#include "main.h"

// Integer types
typedef uint_least16_t u16;
typedef uint_least8_t u8;

struct Z80 {
	// Main registers
	u8 regA, regB, regC, regD, regE, regF, regH, regL;
	u16 stackPointer, programCounter;

	// Alternate (shadow) registers
	u8 regA$, regB$, regC$, regD$, regE$, regF$, regH$, regL$;

	// Index registers
	u16 regIX, regIY;

	// Other registers
	u8 interruptVector;
	u8 refreshCounter;
	u8 statusFlags;
};
struct Z80 CPU;

u8 RAM[0xFFFF] = {0};

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Missing rom.\n");
		printf("Usage: ./emulator [filename]\n");
		return 1;
	} else {
		loadRom(argv[1]);

		return 0;
	}
}

void loadRom(char *fileName) {
	FILE* fp = fopen(fileName, "rb");
	if(fp == NULL) {
		printf("Failed to open file.\n");
		exit(1);
	}

	// Load contents into CPU registers
	CPU.regA = fgetc(fp);
	CPU.regF = fgetc(fp);
	CPU.regC = fgetc(fp);
	CPU.regB = fgetc(fp);
	CPU.regL = fgetc(fp);
	CPU.regH = fgetc(fp);

	fread(&CPU.programCounter, sizeof(u16), 1, fp);
	fread(&CPU.stackPointer, sizeof(u16), 1, fp);

	CPU.interruptVector = fgetc(fp);
	CPU.refreshCounter = fgetc(fp);
	fgetc(fp); // Not sure about this yet (byte 12)
	CPU.regE = fgetc(fp);
	CPU.regD = fgetc(fp);
	CPU.regC$ = fgetc(fp);
	CPU.regB$ = fgetc(fp);
	CPU.regE$ = fgetc(fp);
	CPU.regD$ = fgetc(fp);
	CPU.regL$ = fgetc(fp);
	CPU.regH$ = fgetc(fp);
	CPU.regA$ = fgetc(fp);
	CPU.regF$ = fgetc(fp);

	fread(&CPU.regIY, sizeof(u16), 1, fp);
	fread(&CPU.regIX, sizeof(u16), 1, fp);

	fgetc(fp); // Interrupt flipflop
	fgetc(fp); // IFF2 (not important)
	fgetc(fp); // Mode flags

	int headerLength = 0;
	fread(&headerLength, sizeof(u16), 1, fp);

	fread(&CPU.programCounter, sizeof(u16), 1, fp); // Actual program counter contents

	fgetc(fp); // Hardware mode - currently 48k for this rom
	fgetc(fp); // SamRam mode flag - currently off
	fgetc(fp); // Paged rom flag
	
	/**
	 * Bit 0: 1 if R register emulation on
	 * Bit 1: 1 if LDIR emulation on
	 * Bit 2: AY sound in use, even on 48K machines
	 * Bit 6: (if bit 2 set) Fuller Audio Box emulation
	 * Bit 7: Modify hardware
	 */
	fgetc(fp); // currently bits 0 and 1 are 1

	fgetc(fp); // Sound chip register number
	fseek(fp, 16, SEEK_CUR); // Sound chip register contents
	fseek(fp, 31, SEEK_CUR); // Other flags that I probably need

	// Memory pointer
	u8* memPtr = RAM;

	// Start reading RAM snapshot
	int dataLength = 0;

	fread(&dataLength, sizeof(u16), 1, fp);
	printf("%x\n", dataLength);
	fgetc(fp); // Page number (currently 1)
	memPtr = &RAM[0x8000];
	fread(memPtr, sizeof(u8), dataLength, fp);

	fread(&dataLength, sizeof(u16), 1, fp);
	printf("%x\n", dataLength);
	fgetc(fp); // Page number (currently 2)
	memPtr = &RAM[0xc000];
	fread(memPtr, sizeof(u8), dataLength, fp);

	fread(&dataLength, sizeof(u16), 1, fp);
	printf("%x\n", dataLength);
	fgetc(fp); // Page number (currently 5)
	memPtr = &RAM[0x4000];
	fread(memPtr, sizeof(u8), dataLength, fp);

	exit(0);
}