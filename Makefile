# Makefile for project
# Josephine Nguyen, April 2020

PROG = doc_parser
OBJS = doc_parser.o thread_params.o

CFLAGS = -Wall -pedantic -std=c11 -ggdb -pthread
CC = gcc

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG)

.PHONY: clean

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f stocks
	rm -f core
