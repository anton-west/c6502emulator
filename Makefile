CC=gcc
CFLAGS=-g -Wall -Wextra -pedantic -std=c99
LDFLAGS=-lcmocka

BINDIR=bin/debug
BIN=$(BINDIR)/c6502
SRC=src
OBJ=obj

SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

TEST=tests
TESTBIN=$(TEST)/bin/testprog
TESTS=$(wildcard $(TEST)/*.c)
TESTS+=$(wildcard $(SRC)/*.c)
TESTS:=$(filter-out src/main.c, $(TESTS))
#TESTBINS=$(patsubst $(TEST)/*.c, $(TEST)/bin/%, $(TESTS))

all:$(BIN)
	./$(BIN)

release: cleanrelease
release: BIN=bin/release/c6502
release: CFLAGS=-Wall -O3 -DNDEBUG
release: $(BIN)

# Cannot use $@ as it does not update betweeen debug and release for some reason
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


test: $(TESTBIN)
	./$(TESTBIN)

$(TESTBIN): $(TESTS)
	$(CC) $(CFLAGS) $(TESTS) -o $(TESTBIN) $(LDFLAGS)


.PHONY: clean
clean:
	rm -f bin/debug/* bin/release/* $(OBJ)/* $(TESTBIN)

.PHONY: cleanrelease
cleanrelease:
	rm -f bin/release/* $(OBJ)/* $(TESTBIN)