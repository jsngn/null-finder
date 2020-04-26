#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

typedef struct item {
	char *key;
	void *val;
	struct item *next;
} item_t;

typedef struct slot {
	item_t *head;
} slot_t;


typedef struct hashtable {
	slot_t **slots;
	int slots_n;
} hashtable_t;

static item_t *get_item(slot_t *slot, const char *key);

hashtable_t *hashtable_new(int slots_n)
{
	if (slots_n > 0) {
		hashtable_t *new = malloc(sizeof(hashtable_t));
		if (new == NULL) {
			return NULL;
		}
		
		new->slots_n = slots_n;
		new->slots = calloc(slots_n, sizeof(slot_t*));
		if (new->slots == NULL) {
			free(new);
			return NULL;
		}

		return new;
	}
	else {
		return NULL;
	}
}

int hashtable_insert(hashtable_t *table, char *key, void *val)
{
	if (table != NULL && key != NULL && val != NULL) {
		unsigned long slot_i = jenkins_one_at_a_time_hash(key, table->slots_n);
		
		if (table->slots[slot_i] == NULL) {
			table->slots[slot_i] = malloc(sizeof(slot_t));
			
			if (table->slots[slot_i] == NULL) {
				return 4;
			}

			table->slots[slot_i]->head = NULL;
		}

		if (table->slots[slot_i] != NULL) {
			item_t *new;

			if (get_item(table->slots[slot_i], key) != NULL) {
				return 5;
			}
			
			new = malloc(sizeof(item_t));

			if (new == NULL) {
				return 4;
			}
			
			new->key = key;
			new->val = val;
			new->next = table->slots[slot_i]->head;
			table->slots[slot_i]->head = new;
		}
		else {
			return 4;
		}

	}
	else {
		return 3;
	}

	return 0;
}

static item_t *get_item(slot_t *slot, const char *key)
{
	if (slot != NULL && key != NULL) {
		item_t *found = slot->head;

		while (found != NULL) {
			if (strcmp(found->key, key) == 0) {
				return found;
			}
			found = found->next;
		}

		return NULL;
	}
	else {
		return NULL;
	}
}

void *hashtable_find(hashtable_t *table, const char *key)
{
	if (table != NULL && key != NULL) {
		unsigned long slot_i = jenkins_one_at_a_time_hash(key, table->slots_n);

		if (table->slots[slot_i] != NULL) {
			item_t *found = get_item(table->slots[slot_i], key);
			if (found != NULL) {
				return found->val;
			}
		}
		return NULL;
	}
	else {
		return NULL;
	}
}

void hashtable_free(hashtable_t *table)
{
	if (table != NULL) {
		for (int i = 0; i < table->slots_n; i++) {
			if (table->slots[i] != NULL) {
				item_t *node = table->slots[i]->head;
				
				while (node != NULL) {
					if (node->key != NULL) {
						free(node->key);
					}
					
					if (node->val != NULL) {
						free(node->val);
					}
					
					item_t *next = node->next;
					free(node);
					node = next;
				}

				free(table->slots[i]);
			}
		}
		free(table->slots);
		free(table);
	}
}

void hashtable_print(hashtable_t *table)
{
	if (table != NULL) {
		for (int i = 0; i < table->slots_n; i++) {
			printf("\n%d: ", i);
			if (table->slots[i] != NULL) {
				item_t *node = table->slots[i]->head;

				while (node != NULL) {
					printf("%s, ", node->key);
					node = node->next;
				}
			}
		}
	}
}

unsigned long jenkins_one_at_a_time_hash(const char* key, int size) {
	unsigned long i = 0;
	unsigned hash = 0;
	int len = strlen(key);

	while (i != len) {
		hash += key[i++];
      		hash += (hash << 10);
      		hash ^= (hash >> 6);
    	}
  	
	hash += (hash << 3);
  	hash ^= (hash >> 11);
  	hash += (hash << 15);

  	return hash % size;
}
