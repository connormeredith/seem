#include <stdio.h>
#include "z80.h"

void loadRom(char*, Z80*, u8[]);

// Monitor loader
void _loadMonitorProgram(u8 memory[]);

// ROM loading functions
void _loadRegisters(FILE*, Z80*);
void _loadAdditionalHeaderContents(FILE*, Z80*);
void _loadMemoryBlocks(FILE*, u8[]);

// File functions
int _getNextByte(FILE*);
int _getNextWord(FILE*);