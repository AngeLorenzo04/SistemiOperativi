CC=gcc 
CFLAGS=-Wall -Wextra -Wpedantic -Werror -Wunused -g -std=c17
BIN=$(patsubst %.c,%,$(wildcard *.c))

all: $(BIN)

%: %.c 
	$(CC) $(CFLAGS) -o $@ $<
