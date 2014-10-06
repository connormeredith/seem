#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "cpu.h"

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
	A = fgetc(fp);
	F = fgetc(fp);
	C = fgetc(fp);
	B = fgetc(fp);
	L = fgetc(fp);
	H = fgetc(fp);




	printf("%x\n", A);
	exit(0);
}