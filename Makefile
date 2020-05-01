# Makefile for project
# Josephine Nguyen, April 2020

PROG = find_null
OBJS = find_null.o ./resources/hashtable.o ./resources/csv_data.o ./libcsv/libcsv.o

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I./resources -I./libcsv
CC = gcc

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG)

.PHONY: clean

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f stocks
	rm -f core
