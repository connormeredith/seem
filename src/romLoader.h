#include <stdio.h>
#include "z80.h"

void loadRom(char*, Z80*, u8[]);

void loadRegisters(FILE*, Z80*);
void loadAdditionalHeaderContents(FILE*, Z80*);
void loadMemoryBlocks(FILE*, u8[]);

int getNextByte(FILE*);
int getNextWord(FILE*);