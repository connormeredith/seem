#include <stdio.h>
#include "z80.h"

void loadSnapshot(char*, Z80*, u8[]);

// Version functions.
static void loadVersion1Header(FILE*, Z80*, int);
static void loadVersion2Header(FILE*, Z80*, int);
static void loadVersion3Header(FILE*, Z80*, int);

// Memory loading functions.
static void loadMemory(FILE*, int, int, u8[]);
static void loadVersion1Memory(FILE*, int, u8[]);
static FILE* loadMemoryBlock(FILE*, int, int, u8[]);
static void loadSpectrum48ROM(u8[]);

// Misc snapshot functions.
static void headerError(char*);
static int getSnapshotSize(FILE*);
static u8 getSnapshotVersion(FILE*);
static int getNextWord(FILE*, int);
static int getNextByte(FILE*, int);