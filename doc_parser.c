/*
 * Program to take in a document, parse it using multi-threading, and feed it to an NLP model
 * Josephine Nguyen, April 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int validate_args(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	int stat;
	if ((stat = validate_args(argc, argv)) != 0) {
		return stat;
	}

	return 0;
}

int validate_args(int argc, char *argv[])
{
	if (argc == 1) {
		fprintf(stderr, "Invalid args\n");
		return 1;
	}

	return 0;
}
