CC=gcc
CFLAGS=-g -Wall -Wextra -pedantic
LDFLAGS=-lcmocka

BINDIR=bin
BIN=$(BINDIR)/c6502
SRC=src
OBJ=obj

SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/*.c, $(OBJ)/*.o, $(SRCS))

TEST=tests
TESTS=$(wildcard $(TEST)/*.c)
#TESTBINS=$(patsubst $(TEST)/*.c, $(TEST)/bin/%, $(TESTS))

all:$(BIN)
	./$(BIN)

release: CFLAGS=-Wall -O3 -DNDEBUG
release: clean
release: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


test: /tests/bin/testprog
	./tests/bin/testprog

/tests/bin/testprog: $(TESTS)
	$(CC) $(CFLAGS) $(TESTS) -o tests/bin/testprog $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(BINDIR)/* $(OBJ)/*