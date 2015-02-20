#include <stdlib.h>
#include <stdio.h>
#include "SDL2/SDL.h"

#ifndef arduino
  #include "lib/display.h"
#endif

#include "lib/memory.h"
#include "lib/rom.h"
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

    #ifndef arduino
    SDL_Event event = initDisplay();
      load48KRom();
      loadSnapshot(argv[1], &CPU);
    #endif


    executeOpcode(&CPU, memRead(CPU.pc));
    for(;;) {
      u8 opcode = 0;

      if(CPU.currentTstate >= 69888) {

        while(SDL_PollEvent(&event)) {
          switch(event.type) {
            case SDL_QUIT:
              exit(1);
            default:
              break;
          }
        }
        SDL_Delay(20);
        CPU.currentTstate %= 69888;
        if(CPU.maskableIntEnabled == 1) {
          CPU.currentTstate += 13;
          memWrite(--CPU.sp, (++CPU.pc >> 8));
          memWrite(--CPU.sp, CPU.pc);
          CPU.pc = 0x37;
        }
        #ifndef arduino
          render();
        #endif
      }

      opcode = fetchOpcode(&CPU);
      executeOpcode(&CPU, opcode);
    }
    return 0;
  }
}
