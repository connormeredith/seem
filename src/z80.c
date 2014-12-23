#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "z80.h"
#include "main.h"

u8 bitHexLookup[8] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };
u8 rstHexLookup[8] = { 0x0, 0x8, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38 };
u8* registerHexLookup[8];
u16* registerPairHexLookup[4];
u8* registerPairHexLookupSeparate[4][2];

void init(Z80* cpu) {
  memset ((void *)cpu, 0, sizeof (Z80));

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
  s16 signed16Temp;
  s8 signed8Temp;
  u8 extendedOpcode;
  int temp;

  printf("---- PC::%x - OP::%x\n", cpu->pc, opcode);
  // printf("BC->%x\n", cpu->BC.pair);
  // printf("DE->%x\n", cpu->DE.pair);
  // printf("HL->%x\n", cpu->HL.pair);
  // printf("A->%x\n", cpu->AF.byte.left);
  // printf("F->%x\n", cpu->AF.byte.flags.all);
  // printf("Zero->%x\n", cpu->AF.byte.flags.z);
  // printf("NEG->%x\n", cpu->AF.byte.flags.s);
  // printf("Carry->%x\n", cpu->AF.byte.flags.c);

  switch(opcode) {
    case 0x00: // nop
      cpu->currentTstate += 4;
      break;
    case 0x01: // ld bc, **
    case 0x11: // ld de, **
    case 0x21: // ld hl, **
    // still need to do ld sp, **
      *registerPairHexLookupSeparate[((opcode & 0x30) >> 4)][0] = memory[++cpu->pc];
      *registerPairHexLookupSeparate[((opcode & 0x30) >> 4)][1] = memory[++cpu->pc];
      cpu->currentTstate += 10;
      break;
    case 0x03: // inc bc
    case 0x13: // inc de
    case 0x23: // inc hl
    // still need to do inc sp
      (*registerPairHexLookup[((opcode & 0x30) >> 4)])++;
      cpu->currentTstate += 6;
      break;
    case 0x04: // inc b
    case 0x0C: // inc c
    case 0x14: // inc d
    case 0x1C: // inc e
    case 0x24: // inc h
    case 0x2C: // inc l
    case 0x3C: // inc a
      (*registerHexLookup[((opcode & 0x38) >> 3)])++;
      cpu->AF.byte.flags.s = ((*registerHexLookup[((opcode & 0x38) >> 3)]) < 0);
      cpu->AF.byte.flags.z = ((*registerHexLookup[((opcode & 0x38) >> 3)]) == 0);
      cpu->AF.byte.flags.n = 0;
      cpu->currentTstate += 4;
      break;
    case 0x05: // dec b
    case 0x0D: // dec c
    case 0x15: // dec d
    case 0x1D: // dec e
    case 0x25: // dec h
    case 0x2D: // dec l
    case 0x3D: // dec a
      signed16Temp = (*registerHexLookup[((opcode & 0x38) >> 3)]);
      signed16Temp--;
      // (*registerHexLookup[((opcode & 0x38) >> 3)])--;
      cpu->AF.byte.flags.s = (signed16Temp < 0);
      cpu->AF.byte.flags.z = (signed16Temp == 0);
      (*registerHexLookup[((opcode & 0x38) >> 3)])--;
      cpu->AF.byte.flags.n = 1;
      cpu->currentTstate += 4;
      break;
    case 0x06: // ld b, *
    case 0x0E: // ld c, *
    case 0x16: // ld d, *
    case 0x1E: // ld e, *
    case 0x26: // ld h, *
    case 0x2E: // ld l, *
    case 0x3E: // ld a, *
      *registerHexLookup[((opcode & 0x38) >> 3)] = memory[++cpu->pc];
      cpu->currentTstate += 7;
      break;
    case 0x07: // rlca
      cpu->AF.byte.flags.c = cpu->AF.byte.left & 0x80;
      cpu->AF.byte.left <<= 1;
      cpu->AF.byte.left += (cpu->AF.byte.flags.c);
      cpu->currentTstate += 4;
      break;
    case 0x08: // ex af, af'
      unsigned16Temp = cpu->AF.pair;
      cpu->AF.pair = cpu->_AF.pair;
      cpu->_AF.pair = unsigned16Temp;
      cpu->currentTstate += 4;
      break;
    case 0x09: // add hl, bc
    case 0x19: // add hl, de
    case 0x29: // add hl, hl
    // still need to do add hl, sp
      cpu->HL.pair += *registerPairHexLookup[((opcode & 0x30) >> 4)];
      cpu->currentTstate += 11;
      break;
    case 0x0B: // dec bc
    case 0x1B: // dec de
    case 0x2B: // dec hl
    // still need to do dec sp
      (*registerPairHexLookup[((opcode & 0x30) >> 4)])--;
      cpu->currentTstate += 6;
      break;
    case 0x0F: // rrca
      cpu->AF.byte.flags.c = cpu->AF.byte.left & 0x1;
      cpu->AF.byte.left >>= 1;
      cpu->AF.byte.left += (cpu->AF.byte.flags.c << 7);
      cpu->currentTstate += 4;
      break;
    case 0x10: // djnz
      cpu->BC.byte[1]--;
      if(cpu->BC.byte[1] != 0) {
        signed8Temp = memory[++cpu->pc];
        cpu->pc+=signed8Temp; // Requires signed addition
        cpu->currentTstate += 13;
      } else {
        cpu->pc++; // Skip the jump offset
        cpu->currentTstate += 8;
      }
      break;
    case 0x12: // ld (de), a
      memory[cpu->DE.pair] = cpu->AF.byte.left;
      cpu->currentTstate += 7;
      break;
    case 0x18: // jr *
      signed8Temp = memory[++cpu->pc];
      cpu->pc += signed8Temp;
      cpu->currentTstate += 12;
      break;
    case 0x1A: // ld a, (de)
      cpu->AF.byte.left = memory[cpu->DE.pair];
      cpu->currentTstate += 7;
      break;
    case 0x1F: // rra
      cpu->AF.byte.flags.c = (cpu->AF.byte.left & 0x1);
      cpu->AF.byte.left >>= 1;
      cpu->AF.byte.flags.h = 0;
      cpu->AF.byte.flags.n = 0;
      cpu->currentTstate += 4;
      break;
    case 0x20: // jr nz, *
      if(cpu->AF.byte.flags.z == 0) {
        signed8Temp = memory[++cpu->pc];
        cpu->pc+=signed8Temp; // Requires signed addition
        cpu->currentTstate += 12;
      } else {
        cpu->pc++; // Skip the jump offset
        cpu->currentTstate += 7;
      }
      break;
    case 0x22: // ld **, hl
      unsigned16Temp = memory[++cpu->pc];
      unsigned16Temp += ((u16)memory[++cpu->pc] << 8);
      memory[unsigned16Temp] = cpu->HL.byte[0];
      memory[++unsigned16Temp] = cpu->HL.byte[1];
      cpu->currentTstate += 16;
      break;
    case 0x28: // jr z, *
      if(cpu->AF.byte.flags.z == 1) {
        signed8Temp = memory[++cpu->pc];
        cpu->pc+=signed8Temp; // Requires signed addition
        cpu->currentTstate += 12;
      } else {
        cpu->pc++; // Skip the jump offset
        cpu->currentTstate += 7;
      }
      break;
    case 0x2A: // ld hl, (**)
      unsigned16Temp = memory[++cpu->pc];
      unsigned16Temp += ((u16)memory[++cpu->pc] << 8);
      cpu->HL.byte[0] = memory[unsigned16Temp];
      cpu->HL.byte[1] = memory[unsigned16Temp + 1];
      cpu->currentTstate += 16;
      break;
    case 0x2F: // cpl
      cpu->AF.byte.left = ~(cpu->AF.byte.left);
      cpu->AF.byte.flags.n = 1;
      cpu->currentTstate += 4;
      break;
    case 0x30: // jr nc
      if(cpu->AF.byte.flags.c == 0) {
        signed8Temp = memory[++cpu->pc];
        cpu->pc += signed8Temp;
        cpu->currentTstate += 12;
      } else {
        cpu->pc++;
        cpu->currentTstate += 7;
      }
      break;
    case 0x32: // ld **, a
      unsigned16Temp = memory[++cpu->pc];
      unsigned16Temp += ((u16)memory[++cpu->pc] << 8);
      memory[unsigned16Temp] = cpu->AF.byte.left;
      cpu->currentTstate += 13;
      break;
    case 0x36: // ld (hl), *
      memory[cpu->HL.pair] = memory[++cpu->pc];
      cpu->currentTstate += 10;
      break;
    case 0x37: // SCF
      cpu->AF.byte.flags.n = 0;
      cpu->AF.byte.flags.h = 0;
      cpu->AF.byte.flags.c = 1;
      cpu->currentTstate += 4;
      break;
    case 0x38: // jr c
      if(cpu->AF.byte.flags.c == 1) {
        cpu->pc += memory[++cpu->pc];
        // cpu->pc--;
        cpu->currentTstate += 12;
      } else {
        cpu->pc++;
        cpu->currentTstate += 7;
      }
      break;
    case 0x3A: // ld a, (**)
      unsigned16Temp = memory[++cpu->pc];
      unsigned16Temp += ((u16)memory[++cpu->pc] << 8);
      cpu->AF.byte.left = memory[unsigned16Temp];
      cpu->currentTstate += 13;
      break;
    case 0x3F: // ccf
      cpu->AF.byte.flags.c = !(cpu->AF.byte.flags.c);
      cpu->AF.byte.flags.n = 0;
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
      cpu->currentTstate += 4;
      break;
    case 0x46: // ld b, (hl)
    case 0x4E: // ld c, (hl)
    case 0x56: // ld d, (hl)
    case 0x5E: // ld e, (hl)
    case 0x66: // ld h, (hl)
    case 0x6E: // ld l, (hl)
    case 0x7E: // ld a, (hl)
      *registerHexLookup[((opcode & 0x38) >> 3)] = memory[cpu->HL.pair];
      cpu->currentTstate += 7;
      break;
    case 0x70: // ld (hl), b
    case 0x71: // ld (hl), c
    case 0x72: // ld (hl), d
    case 0x73: // ld (hl), e
    case 0x74: // ld (hl), h
    case 0x75: // ld (hl), l
    case 0x77: // ld (hl), a
      memory[cpu->HL.pair] = *registerHexLookup[(opcode & 0x07)];
      cpu->currentTstate += 7;
      break;
    case 0x80: // add a, b
    case 0x81: // add a, c
    case 0x82: // add a, d
    case 0x83: // add a, e
    case 0x84: // add a, h
    case 0x85: // add a, l
    case 0x87: // add a, a
      unsigned16Temp = (cpu->AF.byte.left + *registerHexLookup[(opcode & 0x07)]);
      cpu->AF.byte.flags.c = (unsigned16Temp > 255);
      cpu->AF.byte.left += *registerHexLookup[(opcode & 0x07)];
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      // cpu->AF.flags.h = (is set when carry from bit 3)
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      cpu->currentTstate += 4;
      break;
    case 0x90: // sub a, b
    case 0x91: // sub a, c
    case 0x92: // sub a, d
    case 0x93: // sub a, e
    case 0x94: // sub a, h
    case 0x95: // sub a, l
    case 0x97: // sub a, a
      signed16Temp = (cpu->AF.byte.left - *registerHexLookup[(opcode & 0x07)]);
      cpu->AF.byte.flags.c = (signed16Temp < 0);
      cpu->AF.byte.left -= *registerHexLookup[(opcode & 0x07)];
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      // cpu->AF.flags.h = (is set when carry from bit 3)
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      cpu->currentTstate += 4;
      break;
    case 0x98: // sbc a, b
    case 0x99: // sbc a, c
    case 0x9A: // sbc a, d
    case 0x9B: // sbc a, e
    case 0x9C: // sbc a, h
    case 0x9D: // sbc a, l
    case 0x9F: // sbc a, a
      signed16Temp = (cpu->AF.byte.left - *registerHexLookup[(opcode & 0x07)]);
      signed16Temp -= cpu->AF.byte.flags.c;
      cpu->AF.byte.flags.c = (signed16Temp < 0);
      cpu->AF.byte.left = signed16Temp;
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      // cpu->AF.flags.h = (is set when carry from bit 3)
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 1;
      cpu->currentTstate += 4;
      break;
    case 0xA0: // AND b
    case 0xA1: // AND c
    case 0xA2: // AND d
    case 0xA3: // AND e
    case 0xA4: // AND h
    case 0xA5: // AND l
    case 0xA7: // AND a
      cpu->AF.byte.left &= *registerHexLookup[(opcode & 0x07)];
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      cpu->AF.byte.flags.h = 1;
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      cpu->AF.byte.flags.c = 0;
      cpu->currentTstate += 4;
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
      cpu->currentTstate += 4;
      break;
    case 0xAE: // XOR (hl)
      cpu->AF.byte.left ^= memory[cpu->HL.pair];
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      cpu->AF.byte.flags.h = 0;
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      cpu->AF.byte.flags.c = 0;
      cpu->currentTstate += 7;
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
      cpu->currentTstate += 4;
      break;
    case 0xB8: // cp b
    case 0xB9: // cp c
    case 0xBA: // cp d
    case 0xBB: // cp e
    case 0xBC: // cp h
    case 0xBD: // cp l
    case 0xBF: // cp a
      signed16Temp = (cpu->AF.byte.left - *registerHexLookup[(opcode & 0x07)]);
      cpu->AF.byte.flags.c = (signed16Temp < 0);
      cpu->AF.byte.flags.s = ((cpu->AF.byte.left - *registerHexLookup[(opcode & 0x07)]) < 0);
      cpu->AF.byte.flags.z = ((cpu->AF.byte.left - *registerHexLookup[(opcode & 0x07)]) == 0);
      // cpu->AF.byte.flags.h = 0; (is set if borrow from bit 4 otherwise reset)
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 1;
      cpu->currentTstate += 4;
      break;
    case 0xC0: // ret nz
      if(cpu->AF.byte.flags.z == 0) {
        cpu->pc = memory[cpu->sp];
        unsigned16Temp = ((u16)memory[++cpu->sp] << 8);
        cpu->sp++;
        cpu->pc += unsigned16Temp;
        --cpu->pc;
        cpu->currentTstate += 11;
      } else {
        cpu->currentTstate += 5;
      }
      break;
    case 0xC1: // pop bc
    case 0xD1: // pop de
    case 0xE1: // pop hl
    case 0xF1: // pop af
      *registerPairHexLookupSeparate[((opcode & 0x30) >> 4)][0] = memory[cpu->sp];
      *registerPairHexLookupSeparate[((opcode & 0x30) >> 4)][1] = memory[++cpu->sp];
      cpu->sp++;
      cpu->currentTstate += 10;
      break;
    case 0xC2: // jp nz
      if(cpu->AF.byte.flags.z != 1) {
        unsigned16Temp = memory[++cpu->pc];
        cpu->pc = (((u16)memory[++cpu->pc] << 8)) + unsigned16Temp;
        cpu->pc--;
        cpu->currentTstate += 12;
      } else {
        cpu->pc += 2;
        cpu->currentTstate += 7;
      }
      break;
    case 0xC3: // jp **
      unsigned16Temp = memory[++cpu->pc];
      unsigned16Temp += ((u16)memory[++cpu->pc] << 8);
      cpu->pc = unsigned16Temp;
      --cpu->pc;
      cpu->currentTstate += 10;
      break;
    case 0xC4: // CALL NZ, **
      if(cpu->AF.byte.flags.z == 0) {
        cpu->pc += 0x3;
        memory[--cpu->sp] = cpu->pc >> 8;
        memory[--cpu->sp] = cpu->pc;
        cpu->pc -= 0x2;
        unsigned16Temp = memory[cpu->pc];
        cpu->pc = ((u16)memory[++cpu->pc] << 8) + unsigned16Temp;
        --cpu->pc;
        cpu->currentTstate += 17;
      } else {
        cpu->pc += 0x2;
        cpu->currentTstate += 10;
      }
      break;
    case 0xC5: // push bc
    case 0xD5: // push de
    case 0xE5: // push hl
    case 0xF5: // push af
      memory[--cpu->sp] = *registerPairHexLookupSeparate[((opcode & 0x30) >> 4)][1];
      memory[--cpu->sp] = *registerPairHexLookupSeparate[((opcode & 0x30) >> 4)][0];
      cpu->currentTstate += 11;
      break;
    case 0xC6: // add a, *
      unsigned16Temp = (cpu->AF.byte.left + memory[++cpu->pc]);
      cpu->AF.byte.flags.c = (unsigned16Temp > 255);
      cpu->AF.byte.left += (memory[cpu->pc]);
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      // cpu->AF.flags.h = (is set if carry from bit 3)
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      cpu->currentTstate += 7;
      break;
    case 0xC7: // rst 00h
    case 0xCF: // rst 08h
    case 0xD7: // rst 10h
    case 0xDF: // rst 18h
    case 0xE7: // rst 20h
    case 0xEF: // rst 28h
    case 0xF7: // rst 30h
    case 0xFF: // rst 38h
      memory[--cpu->sp] = ++cpu->pc >> 8;
      memory[--cpu->sp] = cpu->pc;
      cpu->pc = rstHexLookup[(opcode & 0x38) >> 3];
      cpu->pc--;
      cpu->currentTstate += 11;
      break;
    case 0xC8: // ret z
      if(cpu->AF.byte.flags.z) {
        cpu->pc = memory[cpu->sp];
        unsigned16Temp = ((u16)memory[++cpu->sp] << 8);
        cpu->pc += unsigned16Temp;
        cpu->sp++;
        --cpu->pc;
        cpu->currentTstate += 11;
      } else {
        cpu->currentTstate += 5;
      }
      break;
    case 0xC9: // ret
      cpu->pc = memory[cpu->sp];
      unsigned16Temp = ((u16)memory[++cpu->sp] << 8);
      cpu->sp++;
      cpu->pc += unsigned16Temp;
      --cpu->pc;
      cpu->currentTstate += 10;
      break;
    case 0xCA: // jp z, **
      if(cpu->AF.byte.flags.z != 0) {
        unsigned16Temp = memory[++cpu->pc];
        cpu->pc = ((u16)memory[++cpu->pc] << 8) + unsigned16Temp;
        cpu->pc--;
        cpu->currentTstate += 12;
      } else {
        cpu->pc += 2;
        cpu->currentTstate += 7;
      }
      break;
    case 0xCB: // BIT instruction set
      extendedOpcode = memory[++cpu->pc];
      switch(extendedOpcode) {
        case 0x0E: // rrc (hl)
	  {
	  	u8 tmp;

		  cpu->AF.byte.flags.c = memory[cpu->HL.pair] & 0x1;
		  signed8Temp = memory[cpu->HL.pair];
		  tmp = signed8Temp;
		  signed8Temp >>= 1;
		  signed8Temp += (tmp << 7);
		  cpu->AF.byte.flags.s = (signed8Temp < 0);
		  cpu->AF.byte.flags.z = (signed8Temp == 0);
		  memory[cpu->HL.pair] = signed8Temp;
		  cpu->currentTstate += 15;
	  }
          break;
        case 0x28: // sra b
        case 0x29: // sra c
        case 0x2A: // sra d
        case 0x2B: // sra e
        case 0x2C: // sra h
        case 0x2D: // sra l
        case 0x2F: // sra a
          *registerHexLookup[(extendedOpcode & 0x07)] >>= 1;
          cpu->AF.byte.flags.c = *registerHexLookup[(extendedOpcode & 0x07)] & 1;
          cpu->AF.byte.flags.s = 0;
          cpu->AF.byte.flags.n = 0;
          cpu->AF.byte.flags.z = (*registerHexLookup[(extendedOpcode & 0x07)] == 0);
          cpu->currentTstate += 10;
          break;
        case 0x38: // srl b
        case 0x39: // srl c
        case 0x3A: // srl d
        case 0x3B: // srl e
        case 0x3C: // srl h
        case 0x3D: // srl l
        case 0x3F: // srl a
          cpu->AF.byte.flags.c = *registerHexLookup[(extendedOpcode & 0x07)] & 1;
          *registerHexLookup[(extendedOpcode & 0x07)] >>= 1;
          cpu->AF.byte.flags.s = 0;
          cpu->AF.byte.flags.n = 0;
          cpu->AF.byte.flags.z = (*registerHexLookup[(extendedOpcode & 0x07)] == 0);
          cpu->currentTstate += 10;
          break;
        case 0x40: // bit 0, b
        case 0x48: // bit 1, b
        case 0x50: // bit 2, b
        case 0x58: // bit 3, b
        case 0x60: // bit 4, b
        case 0x68: // bit 5, b
        case 0x70: // bit 6, b
        case 0x78: // bit 7, b
        case 0x41: // bit 0, c
        case 0x49: // bit 1, c
        case 0x51: // bit 2, c
        case 0x59: // bit 3, c
        case 0x61: // bit 4, c
        case 0x69: // bit 5, c
        case 0x71: // bit 6, c
        case 0x79: // bit 7, c
        case 0x42: // bit 0, d
        case 0x4A: // bit 1, d
        case 0x52: // bit 2, d
        case 0x5A: // bit 3, d
        case 0x62: // bit 4, d
        case 0x6A: // bit 5, d
        case 0x72: // bit 6, d
        case 0x7A: // bit 7, d
        case 0x43: // bit 0, e
        case 0x4B: // bit 1, e
        case 0x53: // bit 2, e
        case 0x5B: // bit 3, e
        case 0x63: // bit 4, e
        case 0x6B: // bit 5, e
        case 0x73: // bit 6, e
        case 0x7B: // bit 7, e
        case 0x44: // bit 0, h
        case 0x4C: // bit 1, h
        case 0x54: // bit 2, h
        case 0x5C: // bit 3, h
        case 0x64: // bit 4, h
        case 0x6C: // bit 5, h
        case 0x74: // bit 6, h
        case 0x7C: // bit 7, h
        case 0x45: // bit 0, l
        case 0x4D: // bit 1, l
        case 0x55: // bit 2, l
        case 0x5D: // bit 3, l
        case 0x65: // bit 4, l
        case 0x6D: // bit 5, l
        case 0x75: // bit 6, l
        case 0x7D: // bit 7, l
        case 0x47: // bit 0, a
        case 0x4F: // bit 1, a
        case 0x57: // bit 2, a
        case 0x5F: // bit 3, a
        case 0x67: // bit 4, a
        case 0x6F: // bit 5, a
        case 0x77: // bit 6, a
        case 0x7F: // bit 7, a
          cpu->AF.byte.flags.z = (*registerHexLookup[(extendedOpcode & 0x07)] & bitHexLookup[((extendedOpcode & 0x38) >> 3)]) ? 0 : 1;
          cpu->AF.byte.flags.n = 0;
          cpu->currentTstate += 8;
          break;
        case 0x80: // res 0, b
        case 0x88: // res 1, b
        case 0x90: // res 2, b
        case 0x98: // res 3, b
        case 0xA0: // res 4, b
        case 0xA8: // res 5, b
        case 0xB0: // res 6, b
        case 0xB8: // res 7, b
        case 0x81: // res 0, c
        case 0x89: // res 1, c
        case 0x91: // res 2, c
        case 0x99: // res 3, c
        case 0xA1: // res 4, c
        case 0xA9: // res 5, c
        case 0xB1: // res 6, c
        case 0xB9: // res 7, c
        case 0x82: // res 0, d
        case 0x8A: // res 1, d
        case 0x92: // res 2, d
        case 0x9A: // res 3, d
        case 0xA2: // res 4, d
        case 0xAA: // res 5, d
        case 0xB2: // res 6, d
        case 0xBA: // res 7, d
        case 0x83: // res 0, e
        case 0x8B: // res 1, e
        case 0x93: // res 2, e
        case 0x9B: // res 3, e
        case 0xA3: // res 4, e
        case 0xAB: // res 5, e
        case 0xB3: // res 6, e
        case 0xBB: // res 7, e
        case 0x84: // res 0, h
        case 0x8C: // res 1, h
        case 0x94: // res 2, h
        case 0x9C: // res 3, h
        case 0xA4: // res 4, h
        case 0xAC: // res 5, h
        case 0xB4: // res 6, h
        case 0xBC: // res 7, h
        case 0x85: // res 0, l
        case 0x8D: // res 1, l
        case 0x95: // res 2, l
        case 0x9D: // res 3, l
        case 0xA5: // res 4, l
        case 0xAD: // res 5, l
        case 0xB5: // res 6, l
        case 0xBD: // res 7, l
        case 0x87: // res 0, a
        case 0x8F: // res 1, a
        case 0x97: // res 2, a
        case 0x9F: // res 3, a
        case 0xA7: // res 4, a
        case 0xAF: // res 5, a
        case 0xB7: // res 6, a
        case 0xBF: // res 7, a
          *registerHexLookup[(extendedOpcode & 0x07)] &= ~(bitHexLookup[((extendedOpcode & 0x38) >> 3)]);
          cpu->currentTstate += 8;
          break;
        case 0x86: // res 0, (hl)
        case 0x8E: // res 1, (hl)
        case 0x96: // res 2, (hl)
        case 0x9E: // res 3, (hl)
        case 0xA6: // res 4, (hl)
        case 0xAE: // res 5, (hl)
        case 0xB6: // res 6, (hl)
        case 0xBE: // res 7, (hl)
          memory[(cpu->HL.pair)] &= ~(bitHexLookup[((extendedOpcode & 0x38) >> 3)]);
          cpu->currentTstate += 15;
          break;
        case 0xC0: // set 0, b
        case 0xC8: // set 1, b
        case 0xD0: // set 2, b
        case 0xD8: // set 3, b
        case 0xE0: // set 4, b
        case 0xE8: // set 5, b
        case 0xF0: // set 6, b
        case 0xF8: // set 7, b
        case 0xC1: // set 0, c
        case 0xC9: // set 1, c
        case 0xD1: // set 2, c
        case 0xD9: // set 3, c
        case 0xE1: // set 4, c
        case 0xE9: // set 5, c
        case 0xF1: // set 6, c
        case 0xF9: // set 7, c
        case 0xC2: // set 0, d
        case 0xCA: // set 1, d
        case 0xD2: // set 2, d
        case 0xDA: // set 3, d
        case 0xE2: // set 4, d
        case 0xEA: // set 5, d
        case 0xF2: // set 6, d
        case 0xFA: // set 7, d
        case 0xC3: // set 0, e
        case 0xCB: // set 1, e
        case 0xD3: // set 2, e
        case 0xDB: // set 3, e
        case 0xE3: // set 4, e
        case 0xEB: // set 5, e
        case 0xF3: // set 6, e
        case 0xFB: // set 7, e
        case 0xC4: // set 0, h
        case 0xCC: // set 1, h
        case 0xD4: // set 2, h
        case 0xDC: // set 3, h
        case 0xE4: // set 4, h
        case 0xEC: // set 5, h
        case 0xF4: // set 6, h
        case 0xFC: // set 7, h
        case 0xC5: // set 0, l
        case 0xCD: // set 1, l
        case 0xD5: // set 2, l
        case 0xDD: // set 3, l
        case 0xE5: // set 4, l
        case 0xED: // set 5, l
        case 0xF5: // set 6, l
        case 0xFD: // set 7, l
        case 0xC7: // set 0, a
        case 0xCF: // set 1, a
        case 0xD7: // set 2, a
        case 0xDF: // set 3, a
        case 0xE7: // set 4, a
        case 0xEF: // set 5, a
        case 0xF7: // set 6, a
        case 0xFF: // set 7, a
          *registerHexLookup[(extendedOpcode & 0x07)] |= (bitHexLookup[((extendedOpcode & 0x38) >> 3)]);
          cpu->currentTstate += 8;
          break;
        case 0xC6: // set 0, (hl)
        case 0xCE: // set 1, (hl)
        case 0xD6: // set 2, (hl)
        case 0xDE: // set 3, (hl)
        case 0xE6: // set 4, (hl)
        case 0xEE: // set 5, (hl)
        case 0xF6: // set 6, (hl)
        case 0xFE: // set 7, (hl)
          memory[(cpu->HL.pair)] |= (bitHexLookup[((extendedOpcode & 0x38) >> 3)]);
          cpu->currentTstate += 15;
          break;
        default:
          printf("count -> 0x%x\n", cpu->pc);
          fprintf(stderr, "Unknown BIT opcode -> 0x%x\n", extendedOpcode);
          printf("BC->%x\n", cpu->BC.pair);
          printf("DE->%x\n", cpu->DE.pair);
          printf("HL->%x\n", cpu->HL.pair);
          printf("A->%x\n", cpu->AF.byte.left);
          printf("F->%x\n", cpu->AF.byte.flags.all);
          printf("Zero->%x\n", cpu->AF.byte.flags.z);
          printf("Carry->%x\n", cpu->AF.byte.flags.c);
          printf("Tstate->%i\n", cpu->currentTstate);
          sleep(10000);
          exit(EXIT_FAILURE);
      }
      break;
    case 0xCD: // CALL **
      cpu->pc += 0x3;
      memory[--cpu->sp] = cpu->pc >> 8;
      memory[--cpu->sp] = cpu->pc;
      cpu->pc -= 0x2;
      unsigned16Temp = memory[cpu->pc];
      cpu->pc = ((u16)memory[++cpu->pc] << 8) + unsigned16Temp;
      --cpu->pc;
      cpu->currentTstate += 17;
      break;
    case 0xCE: // adc a, *
      signed16Temp = (cpu->AF.byte.left + memory[++cpu->pc] + cpu->AF.byte.flags.c);
      cpu->AF.byte.left += (memory[cpu->pc] + cpu->AF.byte.flags.c);
      cpu->AF.byte.flags.c = (signed16Temp > 255);
      cpu->AF.byte.flags.s = (signed16Temp < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      // cpu->AF.flags.h = (is set if carry from bit 3)
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      cpu->currentTstate += 7;
      break;
    case 0xD0: // ret nc
      if(cpu->AF.byte.flags.c == 0) {
        cpu->pc = memory[cpu->sp];
        unsigned16Temp = ((u16)memory[++cpu->sp] << 8);
        cpu->sp++;
        cpu->pc += unsigned16Temp;
        --cpu->pc;
        cpu->currentTstate += 11;
      } else {
        cpu->currentTstate += 5;
      }
      break;
    case 0xD2: // jp nc
      if(cpu->AF.byte.flags.c == 0) {
        unsigned16Temp = memory[++cpu->pc];
        cpu->pc = ((u16)memory[++cpu->pc] << 8) + unsigned16Temp;
        cpu->pc--;
        cpu->currentTstate += 12;
      } else {
        cpu->pc += 2;
        cpu->currentTstate += 7;
      }
      break;
    case 0xD6: // sub *
      signed16Temp = (cpu->AF.byte.left - memory[++cpu->pc]);
      cpu->AF.byte.flags.c = (signed16Temp < 0);
      cpu->AF.byte.left = signed16Temp;
      cpu->AF.byte.flags.s = (signed16Temp < 0);
      cpu->AF.byte.flags.z = (signed16Temp == 0);
      // cpu->AF.flags.h = (is set when carry from bit 3)
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      cpu->currentTstate += 7;
      break;
    case 0xD9: // exx
      unsigned16Temp = cpu->BC.pair;
      cpu->BC.pair = cpu->_BC.pair;
      cpu->_BC.pair = unsigned16Temp;
      unsigned16Temp = cpu->DE.pair;
      cpu->DE.pair = cpu->_DE.pair;
      cpu->_DE.pair = unsigned16Temp;
      unsigned16Temp = cpu->HL.pair;
      cpu->HL.pair = cpu->_HL.pair;
      cpu->_HL.pair = unsigned16Temp;
      cpu->currentTstate += 4;
      break;
    case 0xDA: // jp c
      if(cpu->AF.byte.flags.c == 1) {
        unsigned16Temp = memory[++cpu->pc];
        cpu->pc = ((u16)memory[++cpu->pc] << 8) + unsigned16Temp;
        cpu->pc--;
        cpu->currentTstate += 12;
      } else {
        cpu->pc += 2;
        cpu->currentTstate += 7;
      }
      break;
    case 0xDD: // IX instruction set
      extendedOpcode = memory[++cpu->pc];
      switch(extendedOpcode) {
        case 0x09: // add ix, bc
          temp = (cpu->IX.pair + cpu->BC.pair);
          cpu->AF.byte.flags.c = (temp > 65535);
          cpu->IX.pair += cpu->BC.pair;
          cpu->AF.byte.flags.n = 0;
          cpu->currentTstate += 15;
          break;
        case 0x21: // ld ix, **
          cpu->IX.byte[0] = memory[++cpu->pc];
          cpu->IX.byte[1] = memory[++cpu->pc];
          cpu->currentTstate += 14;
          break;
        case 0x22: // ld (**), IX
          unsigned16Temp = memory[++cpu->pc];
          unsigned16Temp += ((u16)memory[++cpu->pc] << 8);
          memory[unsigned16Temp] = cpu->IX.byte[0];
          memory[unsigned16Temp+1] = cpu->IX.byte[1];
          cpu->currentTstate += 20;
          break;
        case 0x23: // inc IX
          cpu->IX.pair += 1;
          cpu->currentTstate += 10;
          break;
        case 0x2A: // ld IX, (**)
          unsigned16Temp = memory[++cpu->pc];
          unsigned16Temp += ((u16)memory[++cpu->pc] << 8);
          cpu->IX.byte[0] = memory[unsigned16Temp];
          cpu->IX.byte[1] = memory[unsigned16Temp + 1];
          cpu->currentTstate += 20;
          break;
        case 0x46: // ld b, (IX+*)
        case 0x4E: // ld c, (IX+*)
        case 0x56: // ld d, (IX+*)
        case 0x5E: // ld e, (IX+*)
        case 0x66: // ld h, (IX+*)
        case 0x6E: // ld l, (IX+*)
        case 0x7E: // ld b, (IX+*)
          *registerHexLookup[((extendedOpcode & 0x38) >> 3)] = memory[(cpu->IX.pair + memory[++cpu->pc])];
          cpu->currentTstate += 19;
          break;
        case 0xE1: // pop IX
          cpu->IX.byte[0] = memory[cpu->sp];
          cpu->IX.byte[1] = memory[++cpu->sp];
          cpu->sp++;
          cpu->currentTstate += 14;
          break;
        case 0xE5: // push IX
          memory[--cpu->sp] = cpu->IX.byte[1];
          memory[--cpu->sp] = cpu->IX.byte[0];
          cpu->currentTstate += 15;
          break;
        case 0xE9: // jp ix
          cpu->pc = cpu->IX.pair;
          cpu->pc--;
          cpu->currentTstate += 8;
          break;
        default:
          printf("count -> 0x%x\n", cpu->pc);
          fprintf(stderr, "Unknown IX opcode -> 0x%x\n", extendedOpcode);
          printf("BC->%x\n", cpu->BC.pair);
          printf("DE->%x\n", cpu->DE.pair);
          printf("HL->%x\n", cpu->HL.pair);
          printf("A->%x\n", cpu->AF.byte.left);
          printf("F->%x\n", cpu->AF.byte.flags.all);
          printf("Zero->%x\n", cpu->AF.byte.flags.z);
          printf("NEG->%x\n", cpu->AF.byte.flags.s);
          printf("Carry->%x\n", cpu->AF.byte.flags.c);
          sleep(10000);
          exit(EXIT_FAILURE);
      }
      break;
    case 0xE3: // ex (sp), HL
      unsigned16Temp = memory[cpu->sp];
      unsigned16Temp += ((u16)memory[(cpu->sp + 1)] << 8);
      memory[cpu->sp] = cpu->HL.byte[1];
      memory[(cpu->sp + 1)] = cpu->HL.byte[0];
      cpu->HL.pair = unsigned16Temp;
      cpu->currentTstate += 4;
      break;
    case 0xE6: // and *
      cpu->AF.byte.left &= memory[++cpu->pc];
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      cpu->AF.byte.flags.h = 1;
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      cpu->AF.byte.flags.c = 0;
      cpu->currentTstate += 7;
      break;
    case 0xE9: // jp (hl)
      cpu->pc = cpu->HL.pair;
      cpu->pc--;
      cpu->currentTstate += 4;
      break;
    case 0xEB: // ex de, HL
      unsigned16Temp = cpu->DE.pair;
      cpu->DE.pair = cpu->HL.pair;
      cpu->HL.pair = unsigned16Temp;
      cpu->currentTstate += 4;
      break;
    case 0xED: // Extended instruction set
      extendedOpcode = memory[++cpu->pc];
      int temp;
      switch(extendedOpcode) {
        case 0x42: // sbc hl, bc
        case 0x52: // sbc hl, de
        case 0x62: // sbc hl, hl
        // case 0x72: // sbc hl, sp
          temp = (cpu->HL.pair - *registerPairHexLookup[(extendedOpcode & 0x30) >> 4]);
          temp -= cpu->AF.byte.flags.c;
          cpu->AF.byte.flags.c = (temp < 0);
          cpu->HL.pair = temp;
          cpu->AF.byte.flags.s = (cpu->HL.pair < 0);
          cpu->AF.byte.flags.z = (cpu->HL.pair == 0);
          // cpu->AF.flags.h = (is set when carry from bit 3)
          // cpu->AF.flags.pv = (is set if overflow)
          cpu->AF.byte.flags.n = 1;
          cpu->currentTstate += 4;
          break;
        case 0x43: // ld (**), bc
          unsigned16Temp = memory[++cpu->pc];
          unsigned16Temp += memory[++cpu->pc] << 8;
          memory[unsigned16Temp] = cpu->BC.byte[0];
          memory[unsigned16Temp+1] = cpu->BC.byte[1];
          cpu->currentTstate += 20;
          break;
        case 0x53: // ld (**), de
          unsigned16Temp = memory[++cpu->pc];
          unsigned16Temp += memory[++cpu->pc] << 8;
          memory[unsigned16Temp] = cpu->DE.byte[0];
          memory[unsigned16Temp+1] = cpu->DE.byte[1];
          cpu->currentTstate += 20;
          break;
        case 0x4B: // ld bc, (**)
        case 0x5B: // ld de, (**)
        case 0x6B: // ld hl, (**)
          unsigned16Temp = memory[++cpu->pc];
          unsigned16Temp += memory[++cpu->pc] << 8;
          *registerPairHexLookupSeparate[((extendedOpcode & 0x30) >> 4)][0] = memory[unsigned16Temp];
          *registerPairHexLookupSeparate[((extendedOpcode & 0x30) >> 4)][1] = memory[unsigned16Temp + 1];
          cpu->currentTstate += 20;
          break;
        case 0x78: // in A, (c)
          cpu->AF.byte.left = memory[(memory[++cpu->pc])];
          cpu->currentTstate += 11;
          break;
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
            cpu->currentTstate += 21;
          } else {
            cpu->currentTstate += 16;
          }
          break;
        default:
          printf("count -> 0x%x\n", cpu->pc);
          fprintf(stderr, "Unknown extended opcode -> 0x%x\n", extendedOpcode);
          printf("BC->%x\n", cpu->BC.pair);
          printf("DE->%x\n", cpu->DE.pair);
          printf("HL->%x\n", cpu->HL.pair);
          printf("A->%x\n", cpu->AF.byte.left);
          printf("F->%x\n", cpu->AF.byte.flags.all);
          printf("Zero->%x\n", cpu->AF.byte.flags.z);
          printf("NEG->%x\n", cpu->AF.byte.flags.s);
          printf("Carry->%x\n", cpu->AF.byte.flags.c);
          sleep(10000);
          exit(EXIT_FAILURE);
      }
      break;
    case 0xF6: // or n
      cpu->AF.byte.left |= memory[++cpu->pc];
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      cpu->AF.byte.flags.h = 0;
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      cpu->AF.byte.flags.c = 0;
      cpu->currentTstate += 7;
      break;
    case 0xFD: // IY instruction set
      extendedOpcode = memory[++cpu->pc];
      switch(extendedOpcode) {
        case 0xBE: // cp a, (IY+d)
          signed16Temp = (cpu->AF.byte.left - memory[(cpu->IY.pair + memory[++cpu->pc])]);
          cpu->AF.byte.flags.c = (signed16Temp < 0);
          cpu->AF.byte.flags.s = (signed16Temp < 0);
          cpu->AF.byte.flags.z = (signed16Temp == 0);
          // cpu->AF.byte.flags.h = 0; (is set if borrow from bit 4 otherwise reset)
          // cpu->AF.flags.pv = (is set if overflow)
          cpu->AF.byte.flags.n = 1;
          cpu->currentTstate += 19;
          break;
        case 0xCB: // IY bit instructions
          signed8Temp = memory[++cpu->pc];
          extendedOpcode = memory[++cpu->pc];
          switch(extendedOpcode) {
            case 0x46: // bit 0, (iy**)
            case 0x4E: // bit 1, (iy**)
            case 0x56: // bit 2, (iy**)
            case 0x5E: // bit 3, (iy**)
            case 0x66: // bit 4, (iy**)
            case 0x6E: // bit 5, (iy**)
            case 0x76: // bit 6, (iy**)
            case 0x7E: // bit 7, (iy**)
              cpu->AF.byte.flags.z = (memory[(cpu->IY.pair + signed8Temp)] & bitHexLookup[((extendedOpcode & 0x38) >> 3)]) ? 0 : 1;
              cpu->currentTstate += 20;
              break;
            case 0x86: // res 0, (iy+*)
            case 0x8E: // res 1, (iy+*)
            case 0x96: // res 2, (iy+*)
            case 0x9E: // res 3, (iy+*)
            case 0xA6: // res 4, (iy+*)
            case 0xAE: // res 5, (iy+*)
            case 0xB6: // res 6, (iy+*)
            case 0xBE: // res 7, (iy+*)
              memory[(cpu->IY.pair + signed8Temp)] &= ~(bitHexLookup[((extendedOpcode & 0x38) >> 3)]);
              cpu->currentTstate += 23;
              break;
            default:
              printf("count -> 0x%x\n", cpu->pc);
              fprintf(stderr, "Unknown IY bit opcode -> 0x%x\n", extendedOpcode);
              printf("BC->%x\n", cpu->BC.pair);
              printf("DE->%x\n", cpu->DE.pair);
              printf("HL->%x\n", cpu->HL.pair);
              printf("A->%x\n", cpu->AF.byte.left);
              printf("F->%x\n", cpu->AF.byte.flags.all);
              printf("Zero->%x\n", cpu->AF.byte.flags.z);
              printf("NEG->%x\n", cpu->AF.byte.flags.s);
              printf("Carry->%x\n", cpu->AF.byte.flags.c);
              sleep(10000);
              exit(EXIT_FAILURE);
          }
          break;
        default:
          printf("count -> 0x%x\n", cpu->pc);
          fprintf(stderr, "Unknown IY opcode -> 0x%x\n", extendedOpcode);
          printf("BC->%x\n", cpu->BC.pair);
          printf("DE->%x\n", cpu->DE.pair);
          printf("HL->%x\n", cpu->HL.pair);
          printf("A->%x\n", cpu->AF.byte.left);
          printf("F->%x\n", cpu->AF.byte.flags.all);
          printf("Zero->%x\n", cpu->AF.byte.flags.z);
          printf("NEG->%x\n", cpu->AF.byte.flags.s);
          printf("Carry->%x\n", cpu->AF.byte.flags.c);
          sleep(10000);
          exit(EXIT_FAILURE);
      }
      break;
    case 0xCC: // call z, **
      if(cpu->AF.byte.flags.z) {
        cpu->pc += 0x3;
        memory[--cpu->sp] = cpu->pc >> 8;
        memory[--cpu->sp] = cpu->pc;
        cpu->pc -= 0x2;
        unsigned16Temp = memory[cpu->pc];
        cpu->pc = (memory[++cpu->pc] << 8) + unsigned16Temp;
        --cpu->pc;
        cpu->currentTstate += 17;
      } else {
        cpu->pc += 0x2;
        cpu->currentTstate += 10;
      }
      break;
    case 0xD3: // out (*), a
      // implement later
      cpu->pc++;
      cpu->currentTstate += 11;
      break;
    case 0xD8: // ret c
      if(cpu->AF.byte.flags.c == 1) {
        cpu->pc = memory[cpu->sp];
        unsigned16Temp = memory[++cpu->sp] << 8;
        cpu->sp++;
        cpu->pc += unsigned16Temp;
        --cpu->pc;
        cpu->currentTstate += 11;
      } else {
        cpu->currentTstate += 5;
      }
      break;
    case 0xEE: // XOR *
      cpu->AF.byte.left ^= memory[++cpu->pc];
      cpu->AF.byte.flags.s = (cpu->AF.byte.left < 0);
      cpu->AF.byte.flags.z = (cpu->AF.byte.left == 0);
      cpu->AF.byte.flags.h = 0;
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 0;
      cpu->AF.byte.flags.c = 0;
      cpu->currentTstate += 7;
      break;
    case 0xF3: // di
      // disables interrupts -- if they were implemented...
      break;
    case 0xF8: // ret m
      if(cpu->AF.byte.flags.s == 1) {
        cpu->pc = memory[cpu->sp];
        unsigned16Temp = memory[++cpu->sp] << 8;
        cpu->sp++;
        cpu->pc += unsigned16Temp;
        --cpu->pc;
        cpu->currentTstate += 11;
      } else {
        cpu->currentTstate += 5;
      }
      break;
    case 0xFB: // ei
      // turns on interrupts again
      break;
    case 0xFE: // cp n
      signed16Temp = cpu->AF.byte.left - memory[++cpu->pc];
      cpu->AF.byte.flags.c = (signed16Temp < 0);
      cpu->AF.byte.flags.s = ((cpu->AF.byte.left - memory[cpu->pc]) < 0);
      cpu->AF.byte.flags.z = ((cpu->AF.byte.left - memory[cpu->pc]) == 0);
      // cpu->AF.byte.flags.h = 0; (is set if borrow from bit 4 otherwise reset)
      // cpu->AF.flags.pv = (is set if overflow)
      cpu->AF.byte.flags.n = 1;
      cpu->currentTstate += 7;
      break;
    default:
      printf("count -> 0x%x\n", cpu->pc);
      fprintf(stderr, "Unknown opcode -> 0x%x\n", opcode);
      printf("BC->%x\n", cpu->BC.pair);
      printf("DE->%x\n", cpu->DE.pair);
      printf("HL->%x\n", cpu->HL.pair);
      printf("A->%x\n", cpu->AF.byte.left);
      printf("F->%x\n", cpu->AF.byte.flags.all);
      printf("Zero->%x\n", cpu->AF.byte.flags.z);
      printf("NEG->%x\n", cpu->AF.byte.flags.s);
      printf("Carry->%x\n", cpu->AF.byte.flags.c);
      sleep(10000);
      exit(EXIT_FAILURE);
  }
}
