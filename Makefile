CC=gcc
CFLAGS=-Wall -Wextra -pedantic
SRC=./src/

c6502: $(SRC)main.c
	$(CC) $(CFLAGS) $(SRC)main.c -o c6502