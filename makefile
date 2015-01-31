# Arduino vars:
DEVICE = atmega2560
CLOCK = 16000000
ARDUINO_OBJECTS = main.o z80.o
ARDUINO_LIB_OBJECTS = memory.o

# x86 vars:
X86_OBJECTS = main.o z80.o snapshot.o
X86_LIB_OBJECTS = memory.o display.o rom.o


# General vars:
LIB_PATH = src/lib

# Architecture selection (defaults to x86)
ifeq ($(ARCH),arduino)
	CC = avr-gcc
	CFLAGS = -Wall -Darduino -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
	LIB_OBJECTS = $(ARDUINO_LIB_OBJECTS)
	OBJECTS = $(ARDUINO_OBJECTS)
else
	ARCH = x86
	CC = gcc
	CFLAGS = -Wall
	LIB_OBJECTS = $(X86_LIB_OBJECTS)
	OBJECTS = $(X86_OBJECTS)
	LIB_FLAGS = `sdl2-config --cflags --libs`
endif

all: clean $(OBJECTS) $(LIB_OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LIB_OBJECTS) -o seem $(LIB_FLAGS)

%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# Arduino builds:
arduino.elf: $(ARDUINO_OBJECTS)
	$(CC) $(CFLAGS) -o main.elf main.o z80.o memory_ar.o

arduino: COMPILER = $(ARDUINO_CC)
arduino: clean arduino.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf

# Shared library objects:
$(LIB_OBJECTS): %.o: $(LIB_PATH)/$(ARCH)/%.c $(LIB_PATH)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# Other:
clean:
	rm -f *.o *.elf *.hex seem