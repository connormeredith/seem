CC=gcc
CFLAGS=-c -Wall

all: emulator

emulator: main.o
	$(CC) main.o -o emulator

main.o: src/main.c
	$(CC) $(CFLAGS) src/main.c

clean:
	rm -rf *o emulator
