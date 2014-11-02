#include <stdio.h>
#include <stdlib.h>

#include "romLoader.h"
#include "Z80.h"

void loadRom(char *filename, Z80* cpu, u8 memory[]) {
	
	_loadMonitorProgram(memory);

	FILE* fp = fopen(filename, "rb");
	if(fp == NULL) {
		printf("Failed to open file.\n");
		//ERROR
	}

	_loadRegisters(fp, cpu);
	_loadAdditionalHeaderContents(fp, cpu);
	_loadMemoryBlocks(fp, memory);

	fclose(fp);
}

void _loadMonitorProgram(u8 memory[]) {
	FILE* fp = fopen("./roms/48.rom", "rb");
	if(fp == NULL) {
		printf("Failed to open file.\n");
		//ERROR
	}

	u16 ramPointer = 0x0;
	int monitorByte;

	while((monitorByte = fgetc(fp)) != EOF) {
		memory[ramPointer++] = monitorByte;
	}

	fclose(fp);
}

/**
 * Populates the CPU registers from a .z80 file.
 * @param fp - The file pointer for the .z80 file.
 * @param cpu - The Z80 struct in which the registers are situated.
 */
void _loadRegisters(FILE* fp, Z80* cpu) {
	if(fseek(fp, 0, SEEK_SET) != 0) {
		fprintf(stderr, "Failed to set file pointer to start of file.\n");
		exit(EXIT_FAILURE);
	}

	// Load main CPU registers
	cpu->AF.left = _getNextByte(fp);
	cpu->AF.flags.all = _getNextByte(fp);
	cpu->BC.byte[0] = _getNextByte(fp);
	cpu->BC.byte[1] = _getNextByte(fp);
	cpu->HL.byte[0] = _getNextByte(fp);
	cpu->HL.byte[1] = _getNextByte(fp);

	// Load program counter and stack pointer
	cpu->pc = _getNextWord(fp);
	cpu->sp = _getNextWord(fp);

	cpu->I = _getNextByte(fp);
	cpu->R = _getNextByte(fp);

	fgetc(fp); // Not sure about this yet (byte 12)

	// Load remaining main CPU registers
	cpu->DE.byte[0] = _getNextByte(fp);
	cpu->DE.byte[1] = _getNextByte(fp);

	// Load shadow registers
	cpu->_BC.byte[0] = _getNextByte(fp);
	cpu->_BC.byte[1] = _getNextByte(fp);
	cpu->_DE.byte[0] = _getNextByte(fp);
	cpu->_DE.byte[1] = _getNextByte(fp);
	cpu->_HL.byte[0] = _getNextByte(fp);
	cpu->_HL.byte[1] = _getNextByte(fp);
	cpu->_AF.left = _getNextByte(fp);
	cpu->_AF.flags.all = _getNextByte(fp);

	// Load index registers
	cpu->IY.pair = _getNextWord(fp);
	cpu->IX.pair = _getNextWord(fp);
}

/**
 * Reads in additional header contents for version 2 and 3 files.
 * @param fp - The file pointer for the .z80 file.
 * @param cpu - The Z80 struct in which the registers are situated.
 */
void _loadAdditionalHeaderContents(FILE* fp, Z80* cpu) {
	// Load actual program counter contents
	if(fseek(fp, 32, SEEK_SET) != 0) {
		fprintf(stderr, "Failed to set file pointer to start of file.\n");
		exit(EXIT_FAILURE);
	}
	cpu->pc = _getNextWord(fp);
}

void _loadMemoryBlocks(FILE* fp, u8 memory[]) {
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

	// We read a maximum of 4 bytes because of compression
	u8 bytes[4] = { 0 };

	int complete = 4;

	while(complete != 1) {

		blockLength = _getNextWord(fp);
		currentPage = _getNextByte(fp);

		// Move the memory pointer to the correct place in memory for this page
		memoryPtr = &memory[pageMapping[currentPage]];

		for (int i = 0; i < blockLength; i++) {
			bytes[0] = _getNextByte(fp);
			if(bytes[0] == 0xED) {
				bytes[1] = _getNextByte(fp);
				if(bytes[1] == 0xED) {
					bytes[2] = _getNextByte(fp);
					bytes[3] = _getNextByte(fp);
					for(int j = 0; j < bytes[2]; j++) {
						*memoryPtr = bytes[3];
						memoryPtr++;
					}
					i+=3;
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
		complete--;
	}
}

int _getNextByte(FILE* fp) {
	int byte = fgetc(fp);
	if(byte == EOF) {
		fprintf(stderr, "Failed to get next byte from file.\n");
		exit(EXIT_FAILURE);
	}
	return byte;
}

int _getNextWord(FILE* fp) {
	int word;
	if(fread(&word, sizeof(u16), 1, fp) != 1) {
		fprintf(stderr, "Failed to get next word from file.\n");
		exit(EXIT_FAILURE);
	}
	return word;
}