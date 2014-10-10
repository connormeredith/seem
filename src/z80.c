#include <stdlib.h>
#include <stdio.h>

#include "z80.h"

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

void executeOpcode(Z80* cpu, u8 memory[], u8 opcode) {
	switch(opcode) {
		case 0xC1:
			break;
		default:
			fprintf(stderr, "Unknown opcode -> 0x%x\n", opcode);
			exit(EXIT_FAILURE);
	}
}