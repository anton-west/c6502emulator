CC=gcc
CFLAGS=-Wall -Wextra -pedantic

objects = main.o cpu.o opcodes.o

all: c6502
	./c6502

c6502: $(objects)
	cc -o c6502 $(objects)

main.o: cpu.h

cpu.o: cpu.h opcodes.h

opcodes.o: cpu.h opcodes.h

.PHONY: clean
clean:
	rm -f *.o c6502