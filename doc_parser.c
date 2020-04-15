/*
 * Program to take in a text document, parse it using multi-threading, and feed it to an NLP model
 * Josephine Nguyen, April 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int validate_args(int argc, char *argv[]);
int read_doc(char *filename, char **words);

int main(int argc, char *argv[])
{
	int stat;
	if ((stat = validate_args(argc, argv)) != 0) {
		return stat;
	}
	
	char **words = calloc(100, sizeof(char*)); // All threads share	
	read_doc(argv[1], words); // Each thread runs this probably

	free(words);
	return 0;
}

int validate_args(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Invalid args.\n");
		return 1;
	}

	return 0;
}

int read_doc(char *filename, char **words)
{
	FILE *fp;
	//char **words = calloc(100, sizeof(char*)); // Starter size, realloc later if needed
	int words_idx = 0;
	char *read = malloc(sizeof(char) * 50);

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "File %s can't be opened for reading.\n", filename);
		return 2;
	}
	
	while (fgets(read, 50, fp) != NULL) {
		char *word = malloc(sizeof(char*) * strlen(read));
		strcpy(word, read);
		words[words_idx] = word;
		printf("%s\n", word);
		words_idx++;
		free(read);
	}

	for (int i = 0; i < words_idx; i++) {
		free(words[words_idx]);
	}

	fclose(fp);
	return 0;
}
