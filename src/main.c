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

	printf("%x\n", CPU.regA);
	printf("%x\n", CPU.regF);
	printf("%x\n", CPU.regC);
	printf("%x\n", CPU.regB);
	printf("%x\n", CPU.regL);
	printf("%x\n", CPU.regH);

	printf("%x\n", CPU.programCounter);
	printf("%i\n", headerLength);
	exit(0);
}