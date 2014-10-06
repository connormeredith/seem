CC=gcc
CFLAGS=-c -Wall

all: emulator

emulator: main.o cpu.o
	$(CC) main.o cpu.o -o emulator

main.o: src/main.c
	$(CC) $(CFLAGS) src/main.c

cpu.o: src/cpu.c
	$(CC) $(CFLAGS) src/cpu.c

clean:
	rm -rf *o emulator
