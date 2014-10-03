#include <stdlib.h>
#include <stdio.h>

#include "../include/loader.h"
#include "../include/main.h"

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Missing rom.\n");
		printf("Usage: ./main [filename]\n");
		return 1;
	} else {
		loadRom(argv[1]);

		return 0;
	}
}

void loadRom(char *fileName) {
	char *rom;

	rom = loadFromFile(fileName);
	if(rom == NULL) {
		printf("An error occurred.\n");
		exit(1);
	}

	if(!validateTZXHeader(rom)) {
		printf("Invalid .TZX header\n");
	}

	
}