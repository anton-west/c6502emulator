CC=gcc
CFLAGS=-g -Wall -Wextra -pedantic -std=c99 
LDFLAGS=-I/opt/homebrew/include -L/opt/homebrew/lib
LDLIBS=-lncurses

BINDIR=bin/debug
BIN=$(BINDIR)/c6502
SRC=src
OBJ=obj

SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))


all: $(BIN)

run: $(BIN)
	./$(BINDIR)/c6502 -f multiply.txt


release: clean
release: BINDIR=bin/release
release: BIN=bin/release/c6502
release: CFLAGS=-Wall -Wextra -O3 -DNDEBUG
release: $(BIN)


# Cannot use $@ as it does not update betweeen debug and release for some reason
$(BIN): $(OBJS)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LDLIBS) $(LDFLAGS) 

$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ)

.PHONY: cleanrelease
cleanrelease:
	rm -f bin/release/* $(OBJ)/*
	rm -rf $(OBJ)
