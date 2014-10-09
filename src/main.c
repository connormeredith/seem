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
		printRAM();
		printf("ProgCount -> 0x%x\n", RAM[CPU.programCounter]);

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