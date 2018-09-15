BIN = bfi
CC = gcc
CELL_T = char
CFLAGS = -O3 -std=c90 -pedantic -DCELL_T=$(CELL_T)

.PHONY: all clean

all: $(BIN)

$(BIN): main.o bf.o
		$(CC) -o $(BIN) main.o bf.o

main.o: main.c bf.h

bf.o: bf.c bf.h

clean:
		rm -rf *.o *.exe $(BIN)
