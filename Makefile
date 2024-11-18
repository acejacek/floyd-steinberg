CFLAGS = -O1 -Wall -std=c99 
LFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

BIN=fs

ALL: main.c
	cc $(CFLAGS) -o $(BIN) main.c $(LFLAGS)

RUN:
run:
	./$(BIN)
