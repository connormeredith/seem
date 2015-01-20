CC = gcc
CFLAGS = -c -Wall

all:
	@echo "Error: No target architecture specified.\nChoose 'make x86' or 'make arduino'"

x86: main.o snapshot.o z80.o display.o memory_x86.o
	$(CC) main.o snapshot.o z80.o display.o memory_x86.o -o emulator `sdl2-config --cflags --libs`

arduino: CC = avr-gcc
arduino: CFLAGS += -Darduino
arduino: main.o z80.o
	$(CC) main.o z80.o -o arduino_emulator

memory_x86.o: src/lib/memory/memory_x86.c src/lib/memory/memory.h
	$(CC) $(CFLAGS) src/lib/memory/memory_x86.c

main.o: src/main.c src/main.h
	$(CC) $(CFLAGS) src/main.c

snapshot.o: src/snapshot.c src/snapshot.h
	$(CC) $(CFLAGS) src/snapshot.c

z80.o: src/z80.c src/z80.h
	$(CC) $(CFLAGS) src/z80.c

display.o: src/display.c src/display.h
	$(CC) $(CFLAGS) src/display.c

clean:
	rm -rf *o emulator
