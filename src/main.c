#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "main.h"
#include "z80.h"

#include "romLoader.h"

// Hardware variables
Z80 CPU;
u8 RAM[0xFFFF] = { 0x00 };

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Missing rom.\n");
		printf("Usage: ./emulator [filename]\n");
		return 1;
	} else {
		initCPU(&CPU);
		loadRom(argv[1], &CPU, RAM);

		printRAM();

		printf("ProgCount -> 0x%x\n", RAM[CPU.programCounter]);

		return 0;
	}
}

// void loadRom(char *fileName) {
// 	FILE* fp = fopen(fileName, "rb");
// 	if(fp == NULL) {
// 		printf("Failed to open file.\n");
// 		exit(1);
// 	}

// 	// Zero all registers
// 	initCPU(CPU);



// 	// // Load contents into CPU registers
// 	// CPU.regA = fgetc(fp);
// 	// CPU.regF = fgetc(fp);
// 	// CPU.regC = fgetc(fp);
// 	// CPU.regB = fgetc(fp);
// 	// CPU.regL = fgetc(fp);
// 	// CPU.regH = fgetc(fp);

// 	// fread(&CPU.programCounter, sizeof(u16), 1, fp);
// 	// fread(&CPU.stackPointer, sizeof(u16), 1, fp);

// 	// CPU.interruptVector = fgetc(fp);
// 	// CPU.refreshCounter = fgetc(fp);
// 	// fgetc(fp); // Not sure about this yet (byte 12)
// 	// CPU.regE = fgetc(fp);
// 	// CPU.regD = fgetc(fp);
// 	// CPU._regC = fgetc(fp);
// 	// CPU._regB = fgetc(fp);
// 	// CPU._regE = fgetc(fp);
// 	// CPU._regD = fgetc(fp);
// 	// CPU._regL = fgetc(fp);
// 	// CPU._regH = fgetc(fp);
// 	// CPU._regA = fgetc(fp);
// 	// CPU._regF = fgetc(fp);

// 	// fread(&CPU.regIY, sizeof(u16), 1, fp);
// 	// fread(&CPU.regIX, sizeof(u16), 1, fp);

// 	// fgetc(fp); // Interrupt flipflop
// 	// fgetc(fp); // IFF2 (not important)
// 	// fgetc(fp); // Mode flags

// 	// int headerLength = 0;
// 	// fread(&headerLength, sizeof(u16), 1, fp);

// 	// fread(&CPU.programCounter, sizeof(u16), 1, fp); // Actual program counter contents

// 	// fgetc(fp); // Hardware mode - currently 48k for this rom
// 	// fgetc(fp); // SamRam mode flag - currently off
// 	// fgetc(fp); // Paged rom flag
	
// 	// /**
// 	//  * Bit 0: 1 if R register emulation on
// 	//  * Bit 1: 1 if LDIR emulation on
// 	//  * Bit 2: AY sound in use, even on 48K machines
// 	//  * Bit 6: (if bit 2 set) Fuller Audio Box emulation
// 	//  * Bit 7: Modify hardware
// 	//  */
// 	// fgetc(fp); // currently bits 0 and 1 are 1

// 	// fgetc(fp); // Sound chip register number
// 	// fseek(fp, 16, SEEK_CUR); // Sound chip register contents
// 	// fseek(fp, 31, SEEK_CUR); // Other flags that I probably need

// 	// // Memory pointer
// 	// u8* memPtr = RAM;

// 	// // Start reading RAM snapshot
// 	// int dataLength = 0;

// 	// /**
// 	//  *  0 - 48K rom
// 	//  *  1 - Interface I, Disciple or Plus D rom
// 	//  *  2 -
// 	//  *  3 -
// 	//  *  4 - 0x8000-bfff
// 	//  *  5 - 0xc000-ffff
// 	//  *  6 -
// 	//  *  7 -
// 	//  *  8 - 0x4000
// 	//  *  9 -
// 	//  * 10 -
// 	//  * 11 -
// 	//  */
// 	// u16 pageStart[12] = {0, 0, 0, 0, 0x8000, 0xc000, 0, 0, 0x4000, 0, 0, 0};
// 	// short currentPage = 0;

// 	// fread(&dataLength, sizeof(u16), 1, fp);
// 	// // printf("%x\n", dataLength);
// 	// currentPage = fgetc(fp);
// 	// // printf("%x\n", currentPage);
// 	// memPtr = &RAM[pageStart[currentPage]];
// 	// // fread(memPtr, sizeof(u8), dataLength, fp);

// 	// u8 bytes[4] = { 0 };
// 	// // Uncompress data
// 	// for (int i = 0; i < dataLength; i++) {
// 	// 	bytes[0] = fgetc(fp);
// 	// 	if(bytes[0] == 0xED) {
// 	// 		bytes[1] = fgetc(fp);
// 	// 		if(bytes[1] == 0xED) {
// 	// 			bytes[2] = fgetc(fp);
// 	// 			bytes[3] = fgetc(fp);
// 	// 			for(int j = 0; j < bytes[2]; j++) {
// 	// 				*memPtr = bytes[3];
// 	// 				memPtr++;
// 	// 			}
// 	// 			i++;
// 	// 			i++;
// 	// 			i++;
// 	// 		} else {
// 	// 			*memPtr = bytes[0];
// 	// 			memPtr++;
// 	// 			*memPtr = bytes[1];
// 	// 			memPtr++;
// 	// 			i++;
// 	// 		}
// 	// 	} else {
// 	// 		*memPtr = bytes[0];
// 	// 		memPtr++;
// 	// 	}
// 	// }

// 	// fread(&dataLength, sizeof(u16), 1, fp);
// 	// // printf("%x\n", dataLength);
// 	// currentPage = fgetc(fp);
// 	// // printf("%x\n", currentPage);
// 	// memPtr = &RAM[pageStart[currentPage]];
// 	// // Uncompress data
// 	// for (int i = 0; i < dataLength; i++) {
// 	// 	bytes[0] = fgetc(fp);
// 	// 	if(bytes[0] == 0xED) {
// 	// 		bytes[1] = fgetc(fp);
// 	// 		if(bytes[1] == 0xED) {
// 	// 			bytes[2] = fgetc(fp);
// 	// 			bytes[3] = fgetc(fp);
// 	// 			for(int j = 0; j < bytes[2]; j++) {
// 	// 				*memPtr = bytes[3];
// 	// 				memPtr++;
// 	// 			}
// 	// 			i++;
// 	// 			i++;
// 	// 			i++;
// 	// 		} else {
// 	// 			*memPtr = bytes[0];
// 	// 			memPtr++;
// 	// 			*memPtr = bytes[1];
// 	// 			memPtr++;
// 	// 			i++;
// 	// 		}
// 	// 	} else {
// 	// 		*memPtr = bytes[0];
// 	// 		memPtr++;
// 	// 	}
// 	// }
// 	// // fread(memPtr, sizeof(u8), dataLength, fp);

// 	// fread(&dataLength, sizeof(u16), 1, fp);
// 	// // printf("%x\n", dataLength);
// 	// currentPage = fgetc(fp);
// 	// // printf("%x\n", currentPage);
// 	// memPtr = &RAM[pageStart[currentPage]];

// 	// for (int i = 0; i < dataLength; i++) {
// 	// 	bytes[0] = fgetc(fp);
// 	// 	if(bytes[0] == 0xED) {
// 	// 		bytes[1] = fgetc(fp);
// 	// 		if(bytes[1] == 0xED) {
// 	// 			bytes[2] = fgetc(fp);
// 	// 			bytes[3] = fgetc(fp);
// 	// 			for(int j = 0; j < bytes[2]; j++) {
// 	// 				*memPtr = bytes[3];
// 	// 				memPtr++;
// 	// 			}
// 	// 			i++;
// 	// 			i++;
// 	// 			i++;
// 	// 		} else {
// 	// 			*memPtr = bytes[0];
// 	// 			memPtr++;
// 	// 			*memPtr = bytes[1];
// 	// 			memPtr++;
// 	// 			i++;
// 	// 		}
// 	// 	} else {
// 	// 		*memPtr = bytes[0];
// 	// 		memPtr++;
// 	// 	}
// 	// }

// 	printf("ProgCount -> 0x%x\n", RAM[CPU.programCounter]);

// 	// fread(memPtr, sizeof(u8), dataLength, fp);
// 	// printRAM();
// 	exit(0);
// }
//485
void printRAM() {
	printf("RAM:\n");
	for (u16 i = 0x4000; i < 0x7fff; i++) {
		printf("-------------------------\n");
		printf("0x%x -> 0x%x\n", i, RAM[i]);
	}
}