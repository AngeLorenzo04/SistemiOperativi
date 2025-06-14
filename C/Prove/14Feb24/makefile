CC=gcc 
CFLAGS=-Wall -Wextra -Wpedantic -Werror -g -std=c11 -D_POSIX_SOURCE
BIN=$(patsubst %.c,%,$(wildcard *.c))

all: $(BIN)

%: %.c 
	$(CC) $(CFLAGS) -o $@ $<