CC=gcc
CFLAGS=-c -Wall

all: emulator

emulator: main.o loader.o
	$(CC) main.o loader.o -o emulator

main.o: src/main.c
	$(CC) $(CFLAGS) src/main.c

loader.o: src/loader.c
	$(CC) $(CFLAGS) src/loader.c

clean:
	rm -rf *o emulator
