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
		initCPU(&CPU); // Zero CPU registers
		loadRom(argv[1], &CPU, RAM); // Load ROM from file into memory

		// Debugging
		// printRAM();
		// printf("ProgCount -> 0x%x\n", RAM[CPU.programCounter]);
		// printf("StackPointer -> 0x%x\n", RAM[CPU.stackPointer]);
		// printf("RegB -> 0x%x\n", CPU.regB);
		// printf("RegC -> 0x%x\n", CPU.regC);

		executeOpcode(&CPU, RAM, RAM[CPU.programCounter]);
		for(;;) {
			// printf("-------------\n");
			u8 opcode = fetchOpcode(&CPU, RAM);

			// if(CPU.programCounter == 0x6F0F) {
			// 	printf("regA - 0x%x\n", CPU.regA);
			// 	printf("regB - 0x%x\n", CPU.regB);
			// 	printf("regC - 0x%x\n", CPU.regC);
			// 	printf("regD - 0x%x\n", CPU.regD);
			// 	printf("regE - 0x%x\n", CPU.regE);

			// 	printf("BREAKPOINT\n");
			// 	break;
			// }

			executeOpcode(&CPU, RAM, opcode);
		}

		printRAM();

		return 0;
	}
}

void printRAM() {
	printf("RAM:\n");
	for (u16 i = 0x4000; i <= 0x7fff; i++) {
		printf("-------------------------\n");
		printf("0x%x -> 0x%x\n", i, RAM[i]);
	}
}