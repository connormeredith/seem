#include <stdlib.h>
#include <stdio.h>

#include "z80.h"
#include "main.h"

void initCPU(Z80* cpu) {
	// cpu->regA = 0x00;
	// cpu->regB = 0x00;
	// cpu->regC = 0x00;
	// cpu->regD = 0x00;
	// cpu->regE = 0x00;
	// cpu->regF = 0x00;
	// cpu->regH = 0x00;
	// cpu->regL = 0x00;

	cpu->stackPointer = 0x0000;
	cpu->programCounter = 0x0000;

	// cpu->regIX = 0x0000;
	// cpu->regIY = 0x0000;

	cpu->interruptVector = 0x00;
	cpu->refreshCounter = 0x00;
	cpu->statusFlags = 0x00;
}

u8 fetchOpcode(Z80* cpu, u8 memory[]) {
	return memory[++(cpu->programCounter)];
}

void executeOpcode(Z80* cpu, u8 ram[], u8 opcode) {
	u16 unsigned16Temp;
	s8 signed8Temp;
	u8 extendedOpcode;

	// printf("--------------\n");
	// printf("count -> 0x%x\n", cpu->programCounter);
	// printf("opcode -> 0x%x\n", opcode);

	switch(opcode) {
		case 0x1: // ld bc, **
			cpu->BC.byte.right = ram[++cpu->programCounter];
			cpu->BC.byte.left = ram[++cpu->programCounter];
			break;
		case 0x6: // ld b, n
			cpu->BC.byte.left = ram[++cpu->programCounter];
			break;
		case 0x10: // djnz
			cpu->BC.byte.left--;
			if(cpu->BC.byte.left != 0) {
				signed8Temp = ram[++cpu->programCounter];
				cpu->programCounter+=signed8Temp; // Requires signed addition
			} else {
				cpu->programCounter++; // Skip the jump offset
			}
			break;
		case 0x11: // ld de, **
			cpu->DE.byte.right = ram[++cpu->programCounter];
			cpu->DE.byte.left = ram[++cpu->programCounter];
			break;
		case 0x21: // ld hl, **
			cpu->HL.byte.right = ram[++cpu->programCounter];
			cpu->HL.byte.left = ram[++cpu->programCounter];
			break;
		case 0x22: // ld **, hl
			unsigned16Temp = ram[++cpu->programCounter];
			unsigned16Temp += ram[++cpu->programCounter] << 8;
			ram[unsigned16Temp] = cpu->HL.byte.left;
			ram[++unsigned16Temp] = cpu->HL.byte.right;
			break;
		case 0x23: // inc hl
			cpu->HL.pair++;
			break;
		case 0x26: // ld h, *
			cpu->HL.byte.left = ram[++cpu->programCounter];
			break;
		case 0x32: // ld **, a
			unsigned16Temp = ram[++cpu->programCounter];
			unsigned16Temp += ram[++cpu->programCounter] << 8;
			ram[unsigned16Temp] = cpu->AF.byte.left;
			break;
		case 0x3E: // ld a, *
			cpu->AF.byte.left = ram[++cpu->programCounter];
			break;
		case 0x56: // ld d, (hl)
			cpu->DE.byte.left = ram[cpu->HL.pair];
			break;
		case 0x5E: // ld e, (hl)
			cpu->DE.byte.right = ram[cpu->HL.pair];
			break;
		case 0x6F: // ld l, a
			cpu->HL.byte.right = cpu->AF.byte.left;
			break;
		case 0x7A: // ld a, d
			cpu->AF.byte.left = cpu->DE.byte.left;
			break;
		case 0x87: // add a, a
			cpu->AF.byte.left += cpu->AF.byte.left;
			break;
		case 0xAF: // XOR A
			cpu->AF.byte.left = cpu->AF.byte.left ^ cpu->AF.byte.left;
			break;
		case 0xB3: // OR e
			cpu->AF.byte.left |= cpu->DE.byte.right;
			break;
		case 0xC1: // pop bc
			cpu->BC.byte.right = ram[cpu->stackPointer];
			cpu->BC.byte.left = ram[++cpu->stackPointer];
			cpu->stackPointer++;
			break;
		case 0xC3: // jp **
			unsigned16Temp = ram[++cpu->programCounter];
			unsigned16Temp += ram[++cpu->programCounter] << 8;
			cpu->programCounter = unsigned16Temp;
			--cpu->programCounter;
			break;
		case 0xC5: // push bc
			ram[--cpu->stackPointer] = cpu->BC.byte.left;
			ram[--cpu->stackPointer] = cpu->BC.byte.right;
			break;
		case 0xC6: // add a, *
			cpu->AF.byte.left += ram[++cpu->programCounter];
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
					ram[--cpu->stackPointer] = cpu->IX.byte.left;
					ram[--cpu->stackPointer] = cpu->IX.byte.right;
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
					ram[cpu->DE.pair] = ram[cpu->HL.pair];
					cpu->HL.pair++;
					cpu->DE.pair++;
					cpu->BC.pair--;

					// Repeat instruction if BC is not zero
					if(cpu->BC.pair != 0) {
						cpu->programCounter -= 2;
					}
					break;
				default:
					printf("count -> 0x%x\n", cpu->programCounter);
					fprintf(stderr, "Unknown extended opcode -> 0x%x\n", extendedOpcode);
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