#ifndef Z80_HEADER
	
	#define Z80_HEADER

	#include "types.h"

	struct z80 {

		// Main registers
		u8 regA, regB, regC, regD, regE, regF, regH, regL;
		u16 stackPointer, programCounter;

		// Shadow registers
		u8 _regA, _regB, _regC, _regD, _regE, _regF, _regH, _regL;

		// Index registers
		u16 regIX, regIY;

		// Other registers
		u8 interruptVector, refreshCounter, statusFlags;
	};

	typedef struct z80 Z80;

	void initCPU(Z80*);

#endif