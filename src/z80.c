#include <stdlib.h>
#include <stdio.h>

#include "z80.h"
#include "main.h"

void initCPU(Z80* cpu) {
	cpu->regA = 0x00;
	cpu->regB = 0x00;
	cpu->regC = 0x00;
	cpu->regD = 0x00;
	cpu->regE = 0x00;
	cpu->regF = 0x00;
	cpu->regH = 0x00;
	cpu->regL = 0x00;

	cpu->stackPointer = 0x0000;
	cpu->programCounter = 0x0000;

	cpu->regIX = 0x0000;
	cpu->regIY = 0x0000;

	cpu->interruptVector = 0x00;
	cpu->refreshCounter = 0x00;
	cpu->statusFlags = 0x00;
}

u8 fetchOpcode(Z80* cpu, u8 memory[]) {
	return memory[++(cpu->programCounter)];
}

void executeOpcode(Z80* cpu, u8 ram[], u8 opcode) {
	u16 unsigned16Temp;
	u16 unsigned16Temp2;
	s8 signed8Temp;
	u8 extendedOpcode;

	// printf("--------------\n");
	// printf("count -> 0x%x\n", cpu->programCounter);
	// printf("opcode -> 0x%x\n", opcode);

	switch(opcode) {
		case 0x1: // ld bc, **
			cpu->regC = ram[++cpu->programCounter];
			cpu->regB = ram[++cpu->programCounter];
			break;
		case 0x6: // ld b, n
			cpu->regB = ram[++cpu->programCounter];
			break;
		case 0x10: // djnz
			cpu->regB--;
			if(cpu->regB != 0) {
				signed8Temp = ram[++cpu->programCounter];
				cpu->programCounter+=signed8Temp; // Requires signed addition
			} else {
				cpu->programCounter++; // Skip the jump offset
			}
			break;
		case 0x11: // ld de, **
			cpu->regE = ram[++cpu->programCounter];
			cpu->regD = ram[++cpu->programCounter];
			break;
		case 0x21: // ld hl, **
			cpu->regL = ram[++cpu->programCounter];
			cpu->regH = ram[++cpu->programCounter];
			break;
		case 0x22: // ld **, hl
			unsigned16Temp = ram[++cpu->programCounter];
			unsigned16Temp += ram[++cpu->programCounter] << 8;
			ram[unsigned16Temp] = cpu->regL;
			ram[++unsigned16Temp] = cpu->regH;
			break;
		case 0x32: // ld **, a
			unsigned16Temp = ram[++cpu->programCounter];
			unsigned16Temp += ram[++cpu->programCounter] << 8;
			ram[unsigned16Temp] = cpu->regA;
			break;
		case 0x3E: // ld a, *
			cpu->regA = ram[++cpu->programCounter];
			break;
		case 0xAF: // XOR A
			cpu->regA = cpu->regA ^ cpu->regA;
			break;
		case 0xC1: // pop bc
			cpu->regC = ram[cpu->stackPointer];
			cpu->regB = ram[++cpu->stackPointer];
			cpu->stackPointer++;
			break;
		case 0xC3: // jp **
			unsigned16Temp = ram[++cpu->programCounter];
			unsigned16Temp += ram[++cpu->programCounter] << 8;
			cpu->programCounter = unsigned16Temp;
			--cpu->programCounter;
			break;
		case 0xC5: // push bc
			ram[--cpu->stackPointer] = cpu->regB;
			ram[--cpu->stackPointer] = cpu->regC;
			break;
		case 0xC9: // ret
			cpu->programCounter = ram[cpu->stackPointer];
			unsigned16Temp = ram[++cpu->stackPointer] << 8;
			cpu->stackPointer++;
			cpu->programCounter += unsigned16Temp;
			--cpu->programCounter;
			break;
		case 0xCD: // CALL **
			cpu->programCounter += 0x3;
			ram[--cpu->stackPointer] = cpu->programCounter >> 8;
			ram[--cpu->stackPointer] = cpu->programCounter;
			cpu->programCounter -= 0x2;
			unsigned16Temp = ram[cpu->programCounter];
			cpu->programCounter = (ram[++cpu->programCounter] << 8) + unsigned16Temp;
			--cpu->programCounter;
			break;
		case 0xDD: // IX instruction set
			extendedOpcode = ram[++cpu->programCounter];
			switch(extendedOpcode) {
				case 0xE5: // push IX
					ram[--cpu->stackPointer] = cpu->regIX >> 8;
					ram[--cpu->stackPointer] = cpu->regIX;
					break;
				default:
					printf("count -> 0x%x\n", cpu->programCounter);
					fprintf(stderr, "Unknown IX opcode -> 0x%x\n", extendedOpcode);
					exit(EXIT_FAILURE);
			}
			break;
		case 0xED: // Extended instruction set
			extendedOpcode = ram[++cpu->programCounter];
			switch(extendedOpcode) {
				case 0xB0: // ldir
					unsigned16Temp = cpu->regH << 8;
					unsigned16Temp += cpu->regL;
					unsigned16Temp2 = cpu->regD << 8;
					unsigned16Temp2 += cpu->regE;
					ram[unsigned16Temp2] = ram[unsigned16Temp];
					
					unsigned16Temp++;
					cpu->regL = unsigned16Temp;
					cpu->regH = unsigned16Temp >> 8;

					unsigned16Temp2++;
					cpu->regE = unsigned16Temp2;
					cpu->regD = unsigned16Temp2 >> 8;

					unsigned16Temp = cpu->regB << 8;
					unsigned16Temp += cpu->regC;
					
					unsigned16Temp--;
					cpu->regC = unsigned16Temp;
					cpu->regB = unsigned16Temp >> 8;

					// Repeat instruction if BC is not zero
					if(unsigned16Temp != 0) {
						cpu->programCounter -= 2;
					}

					break;
				default:
					printf("count -> 0x%x\n", cpu->programCounter);
					fprintf(stderr, "Unknown extended opcode -> 0x%x\n", opcode);
					exit(EXIT_FAILURE);
			}
			break;
		default:
			printf("count -> 0x%x\n", cpu->programCounter);
			fprintf(stderr, "Unknown opcode -> 0x%x\n", opcode);
			// printRAM();
			exit(EXIT_FAILURE);
	}
}