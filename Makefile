CC=gcc
CFLAGS=-g -fpic -O2 -Wall -Wextra -pedantic -std=c99 -c

all: spotless

spotless: main.o scanner.o parser.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o *.so spotless *_wrap.c *.py

.PHONY: all clean
