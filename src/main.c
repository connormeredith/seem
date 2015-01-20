#include <stdlib.h>
#include <stdio.h>

#ifndef arduino
  #include "snapshot.h"
  #include "display.h"
#endif

#include "lib/memory/memory.h"
#include "main.h"
#include "z80.h"

Z80 CPU;

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
    initDisplay();
    loadSnapshot(argv[1], &CPU);
    executeOpcode(&CPU, memRead(CPU.pc));
    for(;;) {
      u8 opcode;
      if(CPU.currentTstate >= 69888) {
        CPU.currentTstate %= 69888;
        opcode = 0xFF;
        render();
      } else {
        opcode = fetchOpcode(&CPU);
      }
      CPU.currentTstate %= 69888;
      executeOpcode(&CPU, opcode);
    }
    return 0;
  }
}
