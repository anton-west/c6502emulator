CC=gcc
CFLAGS=-Wall -Wextra -pedantic
SRC=./src/
BLD=./build/

all: c6502
	./c6502

c6502: main.o cpu.o
	$(CC) $(CFLAGS) -o c6502 main.o cpu.o

main.o: $(SRC)main.c $(SRC)cpu.h
	$(CC) $(CFLAGS) $(SRC)main.c -c

cpu.o: $(SRC)cpu.c
	$(CC) $(CFLAGS) $(SRC)cpu.c -c

.PHONY: clean
clean:
	rm *.o