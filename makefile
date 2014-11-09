CC=gcc
CFLAGS=-c -Wall

all: emulator

emulator: main.o romLoader.o z80.o display.o
	$(CC) main.o romLoader.o z80.o display.o -o emulator `sdl2-config --cflags --libs`

main.o: src/main.c src/main.h
	$(CC) $(CFLAGS) src/main.c

romLoader.o: src/romLoader.c src/romLoader.h
	$(CC) $(CFLAGS) src/romLoader.c

z80.o: src/z80.c src/z80.h
	$(CC) $(CFLAGS) src/z80.c

display.o: src/display.c src/display.h
	$(CC) $(CFLAGS) src/display.c

clean:
	rm -rf *o emulator
