CC=gcc
CFLAGS=-g -O0 -Wall -Wextra  -pedantic -std=c99 -c

all: spotless

spotless: main.o scanner.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o spotless

.PHONY: all clean
