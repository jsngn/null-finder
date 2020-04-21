#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct hashtable hashtable_t;

hashtable_t *hashtable_new(int slots_n);

int hashtable_insert(hashtable_t *table, char *key, void *item);

void *hashtable_find(hashtable_t *table, const char *key);

void hashtable_free(hashtable_t *table);

unsigned long jenkins_one_at_a_time_hash(const char* key, int size);

#endif
