# Makefile for project
# Josephine Nguyen, April 2020

PROG = find_null
OBJS = find_null.o ./resources/hashtable.o

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I./resources
CC = gcc

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG)

.PHONY: clean

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f stocks
	rm -f core
