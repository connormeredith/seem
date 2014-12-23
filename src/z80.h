#ifndef Z80_HEADER
#define Z80_HEADER

#include <stdint.h>

// Integer types.
typedef uint_least16_t u16;
typedef int_least16_t s16;
typedef uint_least8_t u8;
typedef int_least8_t s8;

// Register types.
typedef union {
  u8 byte[2];
  u16 pair;
} RegisterPair;

struct TAG_bytefield {
	union {
		struct {
			u8 c : 1;   // Carry.
			u8 unused: 1;     // Not used.
			u8 pv : 1;  // Parity / Overflow.
			u8 n: 1;    // Add / Subtract.
			u8 h : 1;   // Half carry.
			u8 i : 1;   // Interrupt.
			u8 z : 1;   // Zero.
			u8 s : 1;   // Sign.
		};
		u8 all;       // Used for block loading of the flag register.
	} flags;
	u8 left;
} __attribute__ ((packed));

typedef union {
  struct TAG_bytefield byte;
  u16 pair;
} FlagRegister;

// Processor struct.
struct TAG_Z80 {
  FlagRegister AF, _AF;
  RegisterPair BC, _BC, DE, _DE, HL, _HL, IX, _IX, IY, _IY;
  u16 sp, pc;
  u8 I, R;
  int currentTstate;
} __attribute__((packed));

typedef struct TAG_Z80 Z80;

void init(Z80* cpu);

// Processor functions.
u8 fetchOpcode(Z80*, u8[]);
void executeOpcode(Z80*, u8[], u8);

#endif
