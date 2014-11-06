#include <stdlib.h>
#include <stdio.h>

#include "z80.h"
#include "main.h"

u8 bitHexLookup[8] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };
u8* registerHexLookup[8];
u16* registerPairHexLookup[4];
u8* registerPairHexLookupSeparate[4][2];

void init(Z80* cpu) {
  registerHexLookup[0] = &cpu->BC.byte[1];
  registerHexLookup[1] = &cpu->BC.byte[0];
  registerHexLookup[2] = &cpu->DE.byte[1];
  registerHexLookup[3] = &cpu->DE.byte[0];
  registerHexLookup[4] = &cpu->HL.byte[1];
  registerHexLookup[5] = &cpu->HL.byte[0];
  registerHexLookup[7] = &cpu->AF.byte.left;

  registerPairHexLookup[0] = &cpu->BC.pair;
  registerPairHexLookup[1] = &cpu->DE.pair;
  registerPairHexLookup[2] = &cpu->HL.pair;
  registerPairHexLookup[3] = &cpu->AF.pair;

  registerPairHexLookupSeparate[0][0] = &cpu->BC.byte[0];
  registerPairHexLookupSeparate[0][1] = &cpu->BC.byte[1];
  registerPairHexLookupSeparate[1][0] = &cpu->DE.byte[0];
  registerPairHexLookupSeparate[1][1] = &cpu->DE.byte[1];
  registerPairHexLookupSeparate[2][0] = &cpu->HL.byte[0];
  registerPairHexLookupSeparate[2][1] = &cpu->HL.byte[1];
  registerPairHexLookupSeparate[3][0] = &cpu->AF.byte.flags.all;
  registerPairHexLookupSeparate[3][1] = &cpu->AF.byte.left;
}

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
  u8 unsigned8Temp;
  u8 extendedOpcode;

  switch(opcode) {
    case 0x01: // ld bc, **
    case 0x11: // ld de, **
    case 0x21: // ld hl, **
    // still need to do ld sp, **
      *registerPairHexLookupSeparate[((opcode & 0x30) >> 4)][0] = memory[++cpu->pc];
      *registerPairHexLookupSeparate[((opcode & 0x30) >> 4)][1] = memory[++cpu->pc];
      break;
    case 0x03: // inc bc
    case 0x13: // inc de
    case 0x23: // inc hl
    // still need to do inc sp
      (*registerPairHexLookup[((opcode & 0x30) >> 4)])++;
      break;
    case 0x06: // ld b, *
    case 0x16: // ld d, *
    case 0x26: // ld h, *
    case 0x0E: // ld c, *
    case 0x1E: // ld e, *
    case 0x2E: // ld l, *
    case 0x3E: // ld a, *
      *registerHexLookup[((opcode & 0x38) >> 3)] = memory[++cpu->pc];
      break;
    case 0x09: // add hl, bc
    case 0x19: // add hl, de
    case 0x29: // add hl, hl
    // still need to do add hl, sp
      cpu->HL.pair += *registerPairHexLookup[((opcode & 0x30) >> 4)];
      break;
    case 0x0B: // dec bc
    case 0x1B: // dec de
    case 0x2B: // dec hl
    // still need to do dec sp
      (*registerPairHexLookup[((opcode & 0x30) >> 4)])--;
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
    case 0x20: // jr nz, *
      if(cpu->AF.byte.flags.z == 0) {
        signed8Temp = memory[++cpu->pc];
        cpu->pc+=signed8Temp; // Requires signed addition
      } else {
        cpu->pc++; // Skip the jump offset
      }
      break;
    case 0x22: // ld **, hl
      unsigned16Temp = memory[++cpu->pc];
      unsigned16Temp += memory[++cpu->pc] << 8;
      memory[unsigned16Temp] = cpu->HL.byte[0];
      memory[++unsigned16Temp] = cpu->HL.byte[1];
      break;
    case 0x2A: // ld hl, (**)
      cpu->HL.byte[0] = memory[++cpu->pc];
      cpu->HL.byte[1] = memory[++cpu->pc];
      break;
    case 0x32: // ld **, a
      unsigned16Temp = memory[++cpu->pc];
      unsigned16Temp += memory[++cpu->pc] << 8;
      memory[unsigned16Temp] = cpu->AF.byte.left;
      break;
    case 0x40: // ld b, b
    case 0x41: // ld b, c
    case 0x42: // ld b, d
    case 0x43: // ld b, e
    case 0x44: // ld b, h
    case 0x45: // ld b, l
    case 0x47: // ld b, a
    case 0x48: // ld c, b
    case 0x49: // ld c, c
    case 0x4A: // ld c, d
    case 0x4B: // ld c, e
    case 0x4C: // ld c, h
    case 0x4D: // ld c, l
    case 0x4F: // ld c, a
    case 0x50: // ld d, b
    case 0x51: // ld d, c
    case 0x52: // ld d, d
    case 0x53: // ld d, e
    case 0x54: // ld d, h
    case 0x55: // ld d, l
    case 0x57: // ld d, a
    case 0x58: // ld e, b
    case 0x59: // ld e, c
    case 0x5A: // ld e, d
    case 0x5B: // ld e, e
    case 0x5C: // ld e, h
    case 0x5D: // ld e, l
    case 0x5F: // ld e, a
    case 0x60: // ld h, b
    case 0x61: // ld h, c
    case 0x62: // ld h, d
    case 0x63: // ld h, e
    case 0x64: // ld h, h
    case 0x65: // ld h, l
    case 0x67: // ld h, a
    case 0x68: // ld l, b
    case 0x69: // ld l, c
    case 0x6A: // ld l, d
    case 0x6B: // ld l, e
    case 0x6C: // ld l, h
    case 0x6D: // ld l, l
    case 0x6F: // ld l, a
    case 0x78: // ld a, b
    case 0x79: // ld a, c
    case 0x7A: // ld a, d
    case 0x7B: // ld a, e
    case 0x7C: // ld a, h
    case 0x7D: // ld a, l
    case 0x7F: // ld a, a
      *registerHexLookup[((opcode & 0x38) >> 3)] = *registerHexLookup[(opcode & 0x07)];
      break;
    case 0x46: // ld b, (hl)
    case 0x4E: // ld c, (hl)
    case 0x56: // ld d, (hl)
    case 0x5E: // ld e, (hl)
    case 0x66: // ld h, (hl)
    case 0x6E: // ld l, (hl)
    case 0x7E: // ld a, (hl)
      *registerHexLookup[((opcode & 0x38) >> 3)] = memory[cpu->HL.pair];
      break;
    case 0x80: // add a, b
    case 0x81: // add a, c
    case 0x82: // add a, d
    case 0x83: // add a, e
    case 0x84: // add a, h
    case 0x85: // add a, l
    case 0x87: // add a, a
      cpu->AF.byte.left += *registerHexLookup[(opcode & 0x07)];
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      // cpu->AF.flags.h = (is set when carry from bit 3)
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      // cpu->AF.flags.c = (is set if carry from bit 7)
      break;
    case 0xA8: // XOR B
    case 0xA9: // XOR C
    case 0xAA: // XOR D
    case 0xAB: // XOR E
    case 0xAC: // XOR H
    case 0xAD: // XOR L
    case 0xAF: // XOR A
      cpu->AF.byte.left ^= *registerHexLookup[(opcode & 0x07)];
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      cpu->AF.byte.flags.h = 0;
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      cpu->AF.byte.flags.c = 0;
      break;
    case 0xB0: // Or b
    case 0xB1: // Or c
    case 0xB2: // Or d
    case 0xB3: // Or e
    case 0xB4: // Or h
    case 0xB5: // Or l
    case 0xB7: // Or a
      cpu->AF.byte.left |= *registerHexLookup[(opcode & 0x07)];
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      cpu->AF.byte.flags.h = 0;
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      cpu->AF.byte.flags.c = 0;
      break;
    case 0xC1: // pop bc
    case 0xD1: // pop de
    case 0xE1: // pop hl
    case 0xF1: // pop af
      *registerPairHexLookupSeparate[((opcode & 0x30) >> 4)][0] = memory[cpu->sp];
      *registerPairHexLookupSeparate[((opcode & 0x30) >> 4)][1] = memory[++cpu->sp];
      cpu->sp++;
      break;
    case 0xC3: // jp **
      unsigned16Temp = memory[++cpu->pc];
      unsigned16Temp += memory[++cpu->pc] << 8;
      cpu->pc = unsigned16Temp;
      --cpu->pc;
      break;
    case 0xC5: // push bc
    case 0xD5: // push de
    case 0xE5: // push hl
    case 0xF5: // push af
      memory[--cpu->sp] = *registerPairHexLookupSeparate[((opcode & 0x30) >> 4)][1];
      memory[--cpu->sp] = *registerPairHexLookupSeparate[((opcode & 0x30) >> 4)][0];
      break;
    case 0xC6: // add a, *
      signed8Temp = cpu->AF.byte.left + memory[++cpu->pc];
      cpu->AF.byte.left = signed8Temp;
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      // cpu->AF.flags.h = (is set if carry from bit 3)
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      // cpu->AF.flags.c = (is set if carry from bit 7)
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

          cpu->AF.byte.flags.h = 0;
          cpu->AF.byte.flags.pv = 0;
          cpu->AF.byte.flags.n = 0;

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
    case 0xFD: // IY instruction set
      extendedOpcode = memory[++cpu->pc];
      switch(extendedOpcode) {
        case 0xCB: // IY bit instructions
          unsigned8Temp = memory[++cpu->pc];
          extendedOpcode = memory[++cpu->pc];
          switch(extendedOpcode) {
            case 0xA6: // res 4, (iy+*)
              memory[(cpu->IY.pair + unsigned8Temp)] &= bitHexLookup[((extendedOpcode & 0x38) >> 3)];
              break;
            default:
              printf("count -> 0x%x\n", cpu->pc);
              fprintf(stderr, "Unknown IY bit opcode -> 0x%x\n", extendedOpcode);
              exit(EXIT_FAILURE);
          }
          break;
        default:
          printf("count -> 0x%x\n", cpu->pc);
          fprintf(stderr, "Unknown IY opcode -> 0x%x\n", extendedOpcode);
          exit(EXIT_FAILURE);
      }
      break;
    default:
      printf("count -> 0x%x\n", cpu->pc);
      fprintf(stderr, "Unknown opcode -> 0x%x\n", opcode);
      printf("BC->%x\n", cpu->BC.pair);
      printf("DE->%x\n", cpu->DE.pair);
      printf("HL->%x\n", cpu->HL.pair);
      printf("AF->%x\n", cpu->AF.pair);
      exit(EXIT_FAILURE);
  }
}