/*
 * Program to take in a text document, parse it using multi-threading, and feed it to an NLP model
 * Josephine Nguyen, April 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "thread_params.h"

int validate_args(int argc, char *argv[]);
void *read_doc(void *ptr);
int start_threads(int argc, char *argv[], char **words);

int main(int argc, char *argv[])
{
	int stat;
	if ((stat = validate_args(argc, argv)) != 0) {
		return stat;
	}
	
	char **words = calloc(100, sizeof(char*)); // All threads accumulate their results
	//read_doc(argv[1], words); // Each thread runs this probably

	start_threads(argc, argv, words);
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

int start_threads(int argc, char *argv[], char **words)
{
	pthread_t **threads = calloc(1, sizeof(pthread_t*));
	void **returns = calloc(1, sizeof(void*));
	int threads_idx = 0;
	FILE *fp;

	if ((fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "File %s can't be opened for reading.\n", argv[1]);
		return 2;
	}

	// 1st thread as test
	threads[threads_idx] = malloc(sizeof(pthread_t));
	thread_params_t *params = thread_params_new(words, fp);
	if (params == NULL) {
		return 3;
	}
	pthread_create(threads[threads_idx], NULL, read_doc, params);
	
	for (int i = 0; i < threads_idx + 1; i++) {
		pthread_join((*threads[threads_idx]), returns[threads_idx]);
	}
	free(params); // Contents are free'd later
	fclose(fp);
	for (int i = 0; i < threads_idx + 1; i++) {
		if (returns[i] != NULL) {
			free(returns[i]);
		}
	}
	free(returns);
	for (int i = 0; i < threads_idx + 1; i++) {
		if (threads[threads_idx] != NULL) {
			free(threads[threads_idx]);
		}
	}
	free(threads);

	return 0;
}

void *read_doc(void *ptr)
{
	thread_params_t *params = (thread_params_t *)ptr;
	char **words = thread_params_get_words(params);
	FILE *fp = thread_params_get_fp(params); // File we read from
	int words_idx = 0; // Iterate through words char ptr array
	int c; // Each char we read from file, one at a time
	char *word = calloc(50, sizeof(char));
	*word = '\0'; // Just to be safe
	
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

	return NULL;
}
