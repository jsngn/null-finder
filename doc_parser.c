/*
 * Program to take in a text document, parse it using multi-threading, and feed it to an NLP model
 * Josephine Nguyen, April 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int validate_args(int argc, char *argv[]);
int read_doc(char *filename, char **words);

int main(int argc, char *argv[])
{
	int stat;
	if ((stat = validate_args(argc, argv)) != 0) {
		return stat;
	}
	
	char **words = calloc(100, sizeof(char*)); // All threads accumulate their results
	read_doc(argv[1], words); // Each thread runs this probably

	free(words);
	return 0;
}

int validate_args(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Invalid usage.\n");
		return 1;
	}

	return 0;
}

int read_doc(char *filename, char **words)
{
	FILE *fp; // File we read from
	int words_idx = 0; // Iterate through words char ptr array
	int c; // Each char we read from file, one at a time
	char *word = calloc(50, sizeof(char));
	*word = '\0'; // Just to be safe

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "File %s can't be opened for reading.\n", filename);
		return 2;
	}
	
	while ((c = fgetc(fp)) != EOF) {
		if (isspace(c) != 0) { // We've got a word
			if (strlen(word) > 0) { // In case multiple whitespaces btwn words
				*(words+words_idx) = word;
				words_idx++;
				word = calloc(50, sizeof(char));
				*word = '\0';
			}
		}
		else {
			int len_so_far = strlen(word);
			char n = (char) c;
			// Concatenate to built word w/o strange char at end of each char
			*(word+len_so_far) = n;
			*(word+len_so_far+1) = '\0';
		}
	}
	free(word); // For extra word allocated just before EOF

	for (int i = 0; i < words_idx; i++) {
		printf("%ld %s\n", strlen(*(words+i)), *(words+i));
		free(*(words+i));
	}

	fclose(fp);
	return 0;
}
