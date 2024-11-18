CFLAGS = -O1 -Wall -Werror -Wextra -pedantic -std=c99
LFLAGS = -lraylib -lm

BIN=fs

$(BIN): main.c
	gcc $(CFLAGS) -o $(BIN) main.c $(LFLAGS)

run: $(BIN)
	./$(BIN)

clean:
	rm $(BIN)
