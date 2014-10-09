#include <stdio.h>
#include <stdlib.h>

#include "romLoader.h"
#include "Z80.h"

void loadRom(char *filename, Z80* cpu, u8* memory[]) {
	FILE* fp = fopen(filename, "rb");
	if(fp == NULL) {
		printf("Failed to open file.\n");
		//ERROR
	}

	loadRegisters(fp, cpu);
	loadAdditionalHeaderContents(fp, cpu);
	loadMemoryBlocks(fp, memory);
}

/**
 * Populates the CPU registers from a .z80 file.
 * @param fp - The file pointer for the .z80 file.
 * @param cpu - The Z80 struct in which the registers are situated.
 */
void loadRegisters(FILE* fp, Z80* cpu) {
	if(fseek(fp, 0, SEEK_SET) != 0) {
		fprintf(stderr, "Failed to set file pointer to start of file.\n");
		exit(EXIT_FAILURE);
	}

	// Load main CPU registers
	cpu->regA = getNextByte(fp);
	cpu->regF = getNextByte(fp);
	cpu->regC = getNextByte(fp);
	cpu->regB = getNextByte(fp);
	cpu->regL = getNextByte(fp);
	cpu->regH = getNextByte(fp);

	// Load program counter and stack pointer
	cpu->programCounter = getNextWord(fp);
	cpu->stackPointer = getNextWord(fp);

	cpu->interruptVector = getNextByte(fp);
	cpu->refreshCounter = getNextByte(fp);

	fgetc(fp); // Not sure about this yet (byte 12)

	// Load remaining main CPU registers
	cpu->regE = getNextByte(fp);
	cpu->regD = getNextByte(fp);

	// Load shadow registers
	cpu->_regC = getNextByte(fp);
	cpu->_regB = getNextByte(fp);
	cpu->_regE = getNextByte(fp);
	cpu->_regD = getNextByte(fp);
	cpu->_regL = getNextByte(fp);
	cpu->_regH = getNextByte(fp);
	cpu->_regA = getNextByte(fp);
	cpu->_regF = getNextByte(fp);

	// Load index registers
	cpu->regIY = getNextWord(fp);
	cpu->regIX = getNextWord(fp);
}

/**
 * Reads in additional header contents for version 2 and 3 files.
 * @param fp - The file pointer for the .z80 file.
 * @param cpu - The Z80 struct in which the registers are situated.
 */
void loadAdditionalHeaderContents(FILE* fp, Z80* cpu) {
	// Load actual program counter contents
	if(fseek(fp, 32, SEEK_SET) != 0) {
		fprintf(stderr, "Failed to set file pointer to start of file.\n");
		exit(EXIT_FAILURE);
	}
	cpu->programCounter = getNextWord(fp);
}

void loadMemoryBlocks(FILE* fp, u8 memory[]) {
	// Maps page numbers to points in memory - page 4 == 0x8000 in memory
	u16 pageMapping[12] = {0, 0, 0, 0, 0x8000, 0xc000, 0, 0, 0x4000, 0, 0, 0};

	u8* memoryPtr = memory;
	int blockLength;
	short currentPage;

	// Set file pointer to start of first memory block
	if(fseek(fp, 86, SEEK_SET) != 0) {
		fprintf(stderr, "Failed to set file pointer to start of file.\n");
		exit(EXIT_FAILURE);
	}

	// Needs to be in loop to support multiple blocks
	blockLength = getNextWord(fp);
	currentPage = getNextByte(fp);

	// Move the memory pointer to the correct place in memory for this page
	memoryPtr = &memory[pageMapping[currentPage]];

	u8 bytes[4] = { 0 };
	// Uncompress data
	for (int i = 0; i < blockLength; i++) {
		bytes[0] = fgetc(fp);
		if(bytes[0] == 0xED) {
			bytes[1] = fgetc(fp);
			if(bytes[1] == 0xED) {
				bytes[2] = fgetc(fp);
				bytes[3] = fgetc(fp);
				for(int j = 0; j < bytes[2]; j++) {
					*memoryPtr = bytes[3];
					memoryPtr++;
				}
				i++;
				i++;
				i++;
			} else {
				*memoryPtr = bytes[0];
				memoryPtr++;
				*memoryPtr = bytes[1];
				memoryPtr++;
				i++;
			}
		} else {
			*memoryPtr = bytes[0];
			memoryPtr++;
		}
	}
}

int getNextByte(FILE* fp) {
	int byte = fgetc(fp);
	if(byte == EOF) {
		fprintf(stderr, "Failed to get next byte from file.\n");
		exit(EXIT_FAILURE);
	}
	return byte;
}

int getNextWord(FILE* fp) {
	int word;
	if(fread(&word, sizeof(u16), 1, fp) != 1) {
		fprintf(stderr, "Failed to get next word from file.\n");
		exit(EXIT_FAILURE);
	}
	return word;
}