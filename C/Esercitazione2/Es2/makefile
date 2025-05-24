CC=gcc 
CFLAGS=-Wall -Wextra -Wpedantic -Werror -g -std=c17
BIN=$(patsubst %.c,%,$(wildcard *.c))

all: $(BIN)

%: %.c 
	$(CC) $(CFLAGS) -o $@ $<