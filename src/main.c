#include <stdlib.h>
#include <stdio.h>

#include "romLoader.h"
#include "display.h"
#include "main.h"
#include "z80.h"

Z80 CPU;
u8 RAM[0xFFFF] = { 0x00 };

/**
 * Main function.
 *
 * Arguments: ./emulator [path to .Z80 file].
 */
int main(int argc, char **argv) {
  if(argc != 2) {
    printf("Missing rom.\n");
    printf("Usage: ./emulator [filename]\n");
    return 1;
  } else {
    init(&CPU);
    loadRom(argv[1], &CPU, RAM); // Load ROM from file into memory
    // initDisplay(RAM);
    // printRAM();
    executeOpcode(&CPU, RAM, RAM[CPU.pc]);
    for(;;) {
      // if(CPU.currentTstate >= 69888) {
      //   CPU.currentTstate %= 69888;
      //   printf("REDRAW: Tstate=%i\n", CPU.currentTstate);
      //   render(RAM);
      // }
      // if(RAM[0x5c3c] == 1) {
      //   printf("PC=%x - OP=%x\n", CPU.pc, RAM[CPU.pc]);
      // }
      CPU.currentTstate %= 69888;
      u8 opcode = fetchOpcode(&CPU, RAM);
      executeOpcode(&CPU, RAM, opcode);
    }
    return 0;
  }
}

void printRAM() {
  printf("RAM:\n");
  for (u16 i = 0x5c30; i <= 0x5c3F; i++) {
    printf("-------------------------\n");
    printf("0x%x -> 0x%x\n", i, RAM[i]);
  }
}