Spectrum Emulator for Embedded Machines (SEEM)
===
***
The aim of this project is to produce a ZX Spectrum emulator that is able to run across multiple Arduinos, distributing processing serially between each board.

(It can also be compiled to run on Windows, Mac and Linux.)

##### Required Boards:
- CPU and Memory -> [ATMega 2560](http://arduino.cc/en/Main/ArduinoBoardMega2560)
- Graphics -> TBC

### Getting Started - PC:
##### Prerequisites

[SDL v2.0.3](https://www.libsdl.org/) - *This is required for graphics processing and drawing the screen.*

##### Compiling:
```
make
```

This will produce a single executable: `./emulator`.

### Getting Started - Arduino:
TODO