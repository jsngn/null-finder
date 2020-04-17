#ifndef __THREAD_PARAMS_H
#define __THREAD_PARAMS_H

#include <stdio.h>
#include <stdlib.h>

typedef struct thread_params thread_params_t;

thread_params_t *thread_params_new(char **words, FILE *fp);

char **thread_params_get_words(thread_params_t *tp);

FILE *thread_params_get_fp(thread_params_t *tp);

#endif
