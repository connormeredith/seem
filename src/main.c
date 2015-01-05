#include <stdlib.h>
#include <stdio.h>

#include "snapshot.h"
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
    initDisplay(RAM);
    loadSnapshot(argv[1], &CPU, RAM);
    executeOpcode(&CPU, RAM, RAM[CPU.pc]);
    for(;;) {
      u8 opcode;
      if(CPU.currentTstate >= 69888) {
        CPU.currentTstate %= 69888;
        opcode = 0xFF;
        render(RAM);
      } else {
        opcode = fetchOpcode(&CPU, RAM);
      }
      CPU.currentTstate %= 69888;
      executeOpcode(&CPU, RAM, opcode);
    }
    return 0;
  }
}
