#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/loader.h"

char * loadFromFile(char * fileName) {
	FILE *fp;
	char *fileContents;
	long fileLength;

	fp = fopen(fileName, "rb");
	if(fp == NULL) {
		return NULL;
	}

	fileLength = getFileLength(fp);
	if(fileLength == -1) {
		return NULL;
	}

	fileContents = (char *)malloc((fileLength + 1) * sizeof(char));
	fread(fileContents, fileLength, 1, fp);
	fclose(fp);

	return fileContents;
}

long getFileLength(FILE *fp) {
	long fileLength;

	fseek(fp, 0, SEEK_END);
	fileLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	return fileLength;
}

int validateTZXHeader(char *rom) {
	int match = strncmp("ZXTape!\x1A", rom, 8);
	if(match != 0) {
		return 0;
	}
	return 1;
}