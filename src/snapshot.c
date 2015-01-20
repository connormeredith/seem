#include <stdlib.h>
#include <stdio.h>

#include "snapshot.h"
#include "memory.h"
#include "Z80.h"

void loadSnapshot(char *filename, Z80* cpu) {
	printf("Loading ROM...\n");
	FILE *fp = NULL;
  if((fp = fopen(filename, "rb")) == NULL) {
    printf(" Failed to open %s\n", filename);
    exit(EXIT_FAILURE);
  }

  int snapshotSize = getSnapshotSize(fp);
  printf(" > Snapshot size: %i\n", snapshotSize);
  if(snapshotSize < 30) headerError("Snapshot size is too small.");

  u8 snapshotVersion = getSnapshotVersion(fp);
  printf(" > Snapshot version: %i\n", snapshotVersion);

  // u8 spectrumVersion = 48; // Default to 48K spectrum.
  int memoryBlocksOffset = 30;

  loadVersion1Header(fp, cpu, snapshotSize);
  if(snapshotVersion == 2 || snapshotVersion == 3) {
  	memoryBlocksOffset = 55;
  	loadVersion2Header(fp, cpu, snapshotSize);
  }
  if(snapshotVersion == 3) {
  	memoryBlocksOffset = 86;
  	loadVersion3Header(fp, cpu, snapshotSize);
  }

  printf(" =========================\n");
  printf(" > A register: %x\n", cpu->AF.byte.left);
  printf(" > F register: %x\n", cpu->AF.byte.flags.all);
  printf(" > BC register pair: %x\n", cpu->BC.pair);
  printf(" > DE register pair: %x\n", cpu->DE.pair);
  printf(" > HL register pair: %x\n", cpu->HL.pair);
  printf(" > Interrupt register: %x\n", cpu->I);
  printf(" > Refresh register: %x\n", cpu->R);
  printf(" > Program counter: %x\n", cpu->pc);
  printf(" > Stack pointer: %x\n", cpu->sp);
  printf(" =========================\n");

  loadMemory(fp, memoryBlocksOffset, snapshotSize);
  loadSpectrum48ROM();
  fclose(fp);
}

/**
 * Prints error and exits.
 */
static void headerError(char* message) {
	printf("Error: %s\n", message);
	exit(EXIT_FAILURE);
}

/**
 * Returns the size of the .Z80 snapshot file.
 */
static int getSnapshotSize(FILE* fp) {
  fseek(fp, 0, SEEK_END);
  int endPos = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  return endPos;
}

/**
 * Returns the version of the snapshot file.
 */
static u8 getSnapshotVersion(FILE* fp) {
  u16 word = 0;

  fseek(fp, 6, SEEK_SET);
  fread(&word, sizeof(u16), 1, fp);
  if(word != 0) {
    // Version 1 snapshot.
    return 1;
  }

  fseek(fp, 30, SEEK_SET);
  fread(&word, sizeof(u16), 1, fp);
  if(word == 23) {
    // Version 2 snaphot.
    return 2;
  }
  if(word == 54 || word == 55) {
    // Version 3 snapshot.
    return 3;
  }
  headerError("Invalid snapshot version.");
  return 0;
}

static void loadVersion1Header(FILE* fp, Z80* cpu, int snapshotSize) {
	fseek(fp, 0, SEEK_SET);

  // Load main CPU registers
  cpu->AF.byte.left = getNextByte(fp, snapshotSize);
  cpu->AF.byte.flags.all = getNextByte(fp, snapshotSize);
  cpu->BC.byte[0] = getNextByte(fp, snapshotSize);
  cpu->BC.byte[1] = getNextByte(fp, snapshotSize);
  cpu->HL.byte[0] = getNextByte(fp, snapshotSize);
  cpu->HL.byte[1] = getNextByte(fp, snapshotSize);

  // Load program counter and stack pointer
  cpu->pc = getNextWord(fp, snapshotSize);
  cpu->sp = getNextWord(fp, snapshotSize);

  cpu->I = getNextByte(fp, snapshotSize);
  cpu->R = getNextByte(fp, snapshotSize);

  fgetc(fp); // Not sure about this yet (byte 12)

  // Load remaining main CPU registers
  cpu->DE.byte[0] = getNextByte(fp, snapshotSize);
  cpu->DE.byte[1] = getNextByte(fp, snapshotSize);

  // Load shadow registers
  cpu->_BC.byte[0] = getNextByte(fp, snapshotSize);
  cpu->_BC.byte[1] = getNextByte(fp, snapshotSize);
  cpu->_DE.byte[0] = getNextByte(fp, snapshotSize);
  cpu->_DE.byte[1] = getNextByte(fp, snapshotSize);
  cpu->_HL.byte[0] = getNextByte(fp, snapshotSize);
  cpu->_HL.byte[1] = getNextByte(fp, snapshotSize);
  cpu->_AF.byte.left = getNextByte(fp, snapshotSize);
  cpu->_AF.byte.flags.all = getNextByte(fp, snapshotSize);

  // Load index registers
  cpu->IY.pair = getNextWord(fp, snapshotSize);
  cpu->IX.pair = getNextWord(fp, snapshotSize);
}

static void loadVersion2Header(FILE* fp, Z80* cpu, int snapshotSize) {
	fseek(fp, 32, SEEK_SET);
  cpu->pc = getNextWord(fp, snapshotSize);
}

static void loadVersion3Header(FILE* fp, Z80* cpu, int snapshotSize) {
  fseek(fp, 55, SEEK_SET);
  int lowTstate = (getNextWord(fp, snapshotSize) + 1);
  cpu->currentTstate = (((getNextByte(fp, snapshotSize) + 1) % 4) + 1) * 17472 - lowTstate;
}

static void loadMemory(FILE* fp, int memoryBlocksOffset, int snapshotSize) {
	fseek(fp, memoryBlocksOffset, SEEK_SET);

	if(memoryBlocksOffset == 30) {
		loadVersion1Memory(fp, snapshotSize);
	} else {
		while(ftell(fp) < snapshotSize) {
			fp = loadMemoryBlock(fp, ftell(fp), snapshotSize);
		}
	}
}

static void loadVersion1Memory(FILE* fp, int snapshotSize) {
	fseek(fp, 30, SEEK_SET);
	u16 blockLength = snapshotSize - 34;
	u16 memAddress = 0x4000;

  u8 bytes[4] = { 0 };

  int i;
  for (i = 0; i < blockLength; i++) {
    bytes[0] = getNextByte(fp, snapshotSize);
    if(bytes[0] == 0xED) {
      bytes[1] = getNextByte(fp, snapshotSize);
      if(bytes[1] == 0xED) {
        bytes[2] = getNextByte(fp, snapshotSize);
        bytes[3] = getNextByte(fp, snapshotSize);
        int j;
        for(j = 0; j < bytes[2]; j++) {
          memWrite(memAddress, bytes[3]);
          memAddress++;
        }
        i+=3;
      } else {
        memWrite(memAddress, bytes[0]);
        memAddress++;
        memWrite(memAddress, bytes[1]);
        memAddress++;
        i++;
      }
    } else {
      memWrite(memAddress, bytes[0]);
      memAddress++;
    }
  }
}

static FILE* loadMemoryBlock(FILE* fp, int blockStartOffset, int snapshotSize) {
	// Read header.
	fseek(fp, blockStartOffset, SEEK_SET);
	u16 blockLength = getNextWord(fp, snapshotSize);
	u8 currentPage = getNextByte(fp, snapshotSize);
	if((ftell(fp) + blockLength) > snapshotSize) headerError("Memory block is to big for file.");

	// Maps page numbers to points in memory - page 4 == 0x8000 in memory
	u16 pageMapping[12] = {0, 0, 0, 0, 0x8000, 0xc000, 0, 0, 0x4000, 0, 0, 0};

	// Move the memory pointer to the correct place in memory for this page.
	u16 memAddress = pageMapping[currentPage];
	
	// We read a maximum of 4 bytes because of compression
  u8 bytes[4] = { 0 };

  int i;
  for (i = 0; i < blockLength; i++) {
    bytes[0] = getNextByte(fp, snapshotSize);
    if(bytes[0] == 0xED) {
      bytes[1] = getNextByte(fp, snapshotSize);
      if(bytes[1] == 0xED) {
        bytes[2] = getNextByte(fp, snapshotSize);
        bytes[3] = getNextByte(fp, snapshotSize);
        int j;
        for(j = 0; j < bytes[2]; j++) {
          memWrite(memAddress, bytes[3]);
          memAddress++;
        }
        i+=3;
      } else {
        memWrite(memAddress, bytes[0]);
        memAddress++;
        memWrite(memAddress, bytes[1]);
        memAddress++;
        i++;
      }
    } else {
      memWrite(memAddress, bytes[0]);
      memAddress++;
    }
  }
  return fp;
}

static int getNextByte(FILE* fp, int snapshotSize) {
  u8 byte = 0;
  if((ftell(fp) + 1) > snapshotSize) headerError("Snapshot size is too small.");
  fread(&byte, sizeof(u8), 1, fp);
  return byte;
}

static int getNextWord(FILE* fp, int snapshotSize) {
  u16 word = 0;
  if((ftell(fp) + 2) > snapshotSize) headerError("Snapshot size is too small.");
  fread(&word, sizeof(u16), 1, fp);
  return word;
}

static void loadSpectrum48ROM() {
	FILE* fp = fopen("./roms/48.rom", "rb");
  if(fp == NULL) headerError("Failed to open Spectrum 48Rom program.");

  u16 ramPointer = 0x0;
  int monitorByte;

  while((monitorByte = fgetc(fp)) != EOF) {
    memWrite(ramPointer++, monitorByte);
  }

  fclose(fp);
}