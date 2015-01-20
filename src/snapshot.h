#include <stdio.h>
#include "z80.h"

void loadSnapshot(char*, Z80*);

// Version functions.
static void loadVersion1Header(FILE*, Z80*, int);
static void loadVersion2Header(FILE*, Z80*, int);
static void loadVersion3Header(FILE*, Z80*, int);

// Memory loading functions.
static void loadMemory(FILE*, int, int);
static void loadVersion1Memory(FILE*, int);
static FILE* loadMemoryBlock(FILE*, int, int);
static void loadSpectrum48ROM();

// Misc snapshot functions.
static void headerError(char*);
static int getSnapshotSize(FILE*);
static u8 getSnapshotVersion(FILE*);
static int getNextWord(FILE*, int);
static int getNextByte(FILE*, int);