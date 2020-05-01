/* Hashtable data structure .h file
 * See .c file for code
 * Josephine Nguyen, April 2020
 */

#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>

/* Struct definition */
typedef struct hashtable hashtable_t;

/* Initialize a new hashtable
 * @param number of slots for hashtable
 * @return ptr to new hashtable, NULL if error
 */
hashtable_t *hashtable_new(int slots_n);

/* Insert key, item into hashtable
 * @param table hashtable to insert into
 * @param key the key for hashing
 * @param item value associated w/ key
 * @return exit status
 */
int hashtable_insert(hashtable_t *table, char *key, void *item);

/* Finds item associated w/ a key in hashtable
 * @param table hashtable to look in
 * @param key the key to look for
 * @return ptr to item associated w/ key, or NULL on error/key not found
 */
void *hashtable_find(hashtable_t *table, const char *key);

/* Iterate through hashtable, applying func to every item
 * @param table hashtable to iterate through
 * @param data whatever user wants to pass to func
 * @param func function that's applied to every item in table
 */
void hashtable_iterate(hashtable_t *table, void *data, void (*func)(void *data, const char *key, void *item));

/* Frees up everything in hashtable
 * @param table hashtable to free
 */
void hashtable_free(hashtable_t *table);

/* Prints hashtable keys for testing
 * @param table hashtable to print
 */
void hashtable_print(hashtable_t *table);

/* Hash function
 * @param key string used as key
 * @param size size of hashtable
 * @return index to insert into table
 */
unsigned long jenkins_one_at_a_time_hash(const char* key, int size);

#endif
