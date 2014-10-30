#ifndef Z80_HEADER
	
	#define Z80_HEADER

	#include "types.h"

	struct bytePair {
		u8 right;
		u8 left;
	} bytePair;

	union registerPair {
		struct bytePair byte;
		u16 pair;
	} registerPair;

	struct z80 {

		// Main registers
		union registerPair AF, BC, DE, HL, IX, IY;


		// u8 regA, regB, regC, regD, regE, regF, regH, regL;
		u16 stackPointer, programCounter;

		// Shadow registers
		u8 _regA, _regB, _regC, _regD, _regE, _regF, _regH, _regL;

		// Index registers
		// u16 regIX, regIY;

		// Other registers
		u8 interruptVector, refreshCounter, statusFlags;
	};

	typedef struct z80 Z80;

	u8 fetchOpcode(Z80*, u8[]);
	void executeOpcode(Z80*, u8[], u8);

#endif