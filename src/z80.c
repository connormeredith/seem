#include <stdlib.h>
#include <stdio.h>

#include "z80.h"
#include "main.h"

/**
 * Increments the program counter and grabs the next instruction from memory.
 * @param  cpu    The Z80 processor struct.
 * @param  memory The ZX Spectrum's memory array.
 * @return        The next 8 bit opcode.
 */
u8 fetchOpcode(Z80* cpu, u8 memory[]) {
	return memory[++cpu->pc];
}

/**
 * Executes the opcode that is passed into the function.
 * @param cpu    The Z80 processor struct.
 * @param memory The ZX Spectrum's memory array.
 * @param opcode The 8 bit opcode to be executed.
 */
void executeOpcode(Z80* cpu, u8 memory[], u8 opcode) {
	u16 unsigned16Temp;
	s8 signed8Temp;
	u8 extendedOpcode;

	switch(opcode) {
		case 0x1: // ld bc, **
			cpu->BC.byte[0] = memory[++cpu->pc];
			cpu->BC.byte[1] = memory[++cpu->pc];
			break;
		case 0x6: // ld b, n
			cpu->BC.byte[1] = memory[++cpu->pc];
			break;
		case 0x10: // djnz
			cpu->BC.byte[1]--;
			if(cpu->BC.byte[1] != 0) {
				signed8Temp = memory[++cpu->pc];
				cpu->pc+=signed8Temp; // Requires signed addition
			} else {
				cpu->pc++; // Skip the jump offset
			}
			break;
		case 0x11: // ld de, **
			cpu->DE.byte[0] = memory[++cpu->pc];
			cpu->DE.byte[1] = memory[++cpu->pc];
			break;
		case 0x20: // jr nz, *

			break;
		case 0x21: // ld hl, **
			cpu->HL.byte[0] = memory[++cpu->pc];
			cpu->HL.byte[1] = memory[++cpu->pc];
			break;
		case 0x22: // ld **, hl
			unsigned16Temp = memory[++cpu->pc];
			unsigned16Temp += memory[++cpu->pc] << 8;
			memory[unsigned16Temp] = cpu->HL.byte[1];
			memory[++unsigned16Temp] = cpu->HL.byte[0];
			break;
		case 0x23: // inc hl
			cpu->HL.pair++;
			break;
		case 0x26: // ld h, *
			cpu->HL.byte[0] = memory[++cpu->pc];
			break;
		case 0x32: // ld **, a
			unsigned16Temp = memory[++cpu->pc];
			unsigned16Temp += memory[++cpu->pc] << 8;
			memory[unsigned16Temp] = cpu->AF.left;
			break;
		case 0x3E: // ld a, *
			cpu->AF.left = memory[++cpu->pc];
			break;
		case 0x56: // ld d, (hl)
			cpu->DE.byte[1] = memory[cpu->HL.pair];
			break;
		case 0x5E: // ld e, (hl)
			cpu->DE.byte[0] = memory[cpu->HL.pair];
			break;
		case 0x6F: // ld l, a
			cpu->HL.byte[0] = cpu->AF.left;
			break;
		case 0x7A: // ld a, d
			cpu->AF.left = cpu->DE.byte[1];
			break;
		case 0x87: // add a, a
			cpu->AF.left += cpu->AF.left;
			break;
		case 0xAF: // XOR A
			cpu->AF.left = cpu->AF.left ^ cpu->AF.left;
			break;
		case 0xB3: // OR e
			cpu->AF.left |= cpu->DE.byte[0];
			break;
		case 0xC1: // pop bc
			cpu->BC.byte[0] = memory[cpu->sp];
			cpu->BC.byte[1] = memory[++cpu->sp];
			cpu->sp++;
			break;
		case 0xC3: // jp **
			unsigned16Temp = memory[++cpu->pc];
			unsigned16Temp += memory[++cpu->pc] << 8;
			cpu->pc = unsigned16Temp;
			--cpu->pc;
			break;
		case 0xC5: // push bc
			memory[--cpu->sp] = cpu->BC.byte[1];
			memory[--cpu->sp] = cpu->BC.byte[0];
			break;
		case 0xC6: // add a, *
			cpu->AF.left += memory[++cpu->pc];
			break;
		case 0xC9: // ret
			cpu->pc = memory[cpu->sp];
			unsigned16Temp = memory[++cpu->sp] << 8;
			cpu->sp++;
			cpu->pc += unsigned16Temp;
			--cpu->pc;
			break;
		case 0xCD: // CALL **
			cpu->pc += 0x3;
			memory[--cpu->sp] = cpu->pc >> 8;
			memory[--cpu->sp] = cpu->pc;
			cpu->pc -= 0x2;
			unsigned16Temp = memory[cpu->pc];
			cpu->pc = (memory[++cpu->pc] << 8) + unsigned16Temp;
			--cpu->pc;
			break;
		case 0xDD: // IX instruction set
			extendedOpcode = memory[++cpu->pc];
			switch(extendedOpcode) {
				case 0xE5: // push IX
					memory[--cpu->sp] = cpu->IX.byte[1];
					memory[--cpu->sp] = cpu->IX.byte[0];
					break;
				default:
					printf("count -> 0x%x\n", cpu->pc);
					fprintf(stderr, "Unknown IX opcode -> 0x%x\n", extendedOpcode);
					exit(EXIT_FAILURE);
			}
			break;
		case 0xED: // Extended instruction set
			extendedOpcode = memory[++cpu->pc];
			switch(extendedOpcode) {
				case 0xB0: // ldir
					memory[cpu->DE.pair] = memory[cpu->HL.pair];
					cpu->HL.pair++;
					cpu->DE.pair++;
					cpu->BC.pair--;

					// Repeat instruction if BC is not zero
					if(cpu->BC.pair != 0) {
						cpu->pc -= 2;
					}
					break;
				default:
					printf("count -> 0x%x\n", cpu->pc);
					fprintf(stderr, "Unknown extended opcode -> 0x%x\n", extendedOpcode);
					exit(EXIT_FAILURE);
			}
			break;
		default:
			printf("count -> 0x%x\n", cpu->pc);
			fprintf(stderr, "Unknown opcode -> 0x%x\n", opcode);
			exit(EXIT_FAILURE);
	}
}