CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -pthread -Iinclude
SRC     = $(wildcard src/*.c)
SRC_LIB = $(filter-out src/main.c,$(SRC))
BIN     = simulador

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

test: tests/test_mmu.c $(SRC_LIB)
	$(CC) $(CFLAGS) tests/test_mmu.c $(SRC_LIB) -o test_mmu
	./test_mmu

clean:
	rm -f $(BIN) test_mmu

.PHONY: all test clean
