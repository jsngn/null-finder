#include <stdio.h>
#include <stdlib.h>
#include "thread_params.h"

typedef struct thread_params {
	char **words;
	FILE *fp;
} thread_params_t;


thread_params_t *thread_params_new(char **words, FILE *fp)
{
	if (words == NULL || fp == NULL) {
		return NULL;
	}

	thread_params_t *new = malloc(sizeof(thread_params_t));

	if (new == NULL) {
		return NULL;
	}

	new->words = words;
	new->fp = fp;

	return new;
}

char **thread_params_get_words(thread_params_t *tp)
{
	if (tp == NULL) {
		return NULL;
	}

	return tp->words;
}

FILE *thread_params_get_fp(thread_params_t *tp)
{
	if (tp == NULL) {
		return NULL;
	}

	return tp->fp;
}
