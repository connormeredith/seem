CC=gcc
CFLAGS=-c -Wall

all: emulator

emulator: main.o romLoader.o z80.o
	$(CC) main.o romLoader.o z80.o -o emulator

main.o: src/main.c
	$(CC) $(CFLAGS) src/main.c

romLoader.o: src/romLoader.c
	$(CC) $(CFLAGS) src/romLoader.c

z80.o: src/z80.c
	$(CC) $(CFLAGS) src/z80.c

clean:
	rm -rf *o emulator
