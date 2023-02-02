CC=gcc
CFLAGS=-g -Wall -Wextra -pedantic -std=c99 
LDFLAGS=-I/opt/homebrew/include -L/opt/homebrew/lib
LDLIBS=-lncurses -lcmocka

BINDIR=bin/debug
BIN=$(BINDIR)/c6502
SRC=src
OBJ=obj

SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

# Setup testing
TEST=tests
TESTBIN=$(TEST)/bin/testprog
TESTS=$(wildcard $(TEST)/*.c)
TESTS+=$(wildcard $(SRC)/*.c)
TESTS:=$(filter-out src/main.c, $(TESTS))
TESTS:=$(filter-out src/display.c, $(TESTS))
#TESTBINS=$(patsubst $(TEST)/*.c, $(TEST)/bin/%, $(TESTS))


all: $(BIN)

run: $(BIN)
	./$(BINDIR)/c6502 -f multiply.txt


release: cleanrelease
release: BIN=bin/release/c6502
release: CFLAGS=-Wall -O3 -DNDEBUG
release: $(BIN)


# Cannot use $@ as it does not update betweeen debug and release for some reason
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LDLIBS) $(LDFLAGS) 

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


test: $(TESTBIN)
	./$(TESTBIN)

$(TESTBIN): $(TESTS)
	$(CC) $(CFLAGS) $(TESTS) -o $(TESTBIN) $(LDLIBS) $(LDFLAGS)


.PHONY: clean
clean:
	rm -f bin/debug/* bin/release/* $(OBJ)/* $(TESTBIN)

.PHONY: cleanrelease
cleanrelease:
	rm -f bin/release/* $(OBJ)/* $(TESTBIN)