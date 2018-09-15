BIN = bfc
CC = gcc
CFLAGS = -O3 -std=c89 -pedantic

.PHONY: all clean

all: $(BIN)

$(BIN): main.o bf.o
		$(CC) -o $(BIN) main.o bf.o

main.o: main.c bf.h

bf.o: bf.c bf.h

clean:
		rm -rf *.o *.exe $(BIN)
