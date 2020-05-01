/* Struct to hold all info about a csv needed for detecting null-equivalent words in file
 * The .c file
 * Josephine Nguyen, April 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include "csv_data.h"
#include "hashtable.h"

typedef struct csv_data {
	int rows_n; // Num of rows in file
	int cols_n; // Num of cols in file
	int col_curr; // Current column # we're processing (can be for anything: reading fields, iterating through hashtable items, etc.)
	hashtable_t **columns; // Each hashtable in array reps a column, in each column table key is field/word, val is freq at
				//which word appears in col (at the end change to probability occur in col)
	hashtable_t **column_to_nulls; // Each hashtable in array reps a column, in each column table word is key, val is dummy item
	char **nulls; // Array of strings that are pre-defined null-equivalent words (found in resources/nulls)
	float **avg_probabilities; // Array of floats, each float is avg probability at which words appear that respective col
				//(0th item is 1st col, so on)
} csv_data_t;

// Local function
static void count_unique_rows(void *arg, const char *key, void *val);

csv_data_t *csv_data_new(char **nulls, int rows)
{
	if (nulls == NULL) { // Array must be prepopulated!!!
		return NULL;
	}

	csv_data_t *new = malloc(sizeof(csv_data_t));
	if (new == NULL) {
		return NULL;
	}
	
	new->rows_n = rows;
	new->cols_n = 0;
	new->col_curr = 0;
	new->columns = NULL;
	new->column_to_nulls = NULL;
	new->nulls = nulls;
	new->avg_probabilities = NULL;

	return new;
}

int csv_data_get_rows_n(csv_data_t *csv)
{
	if (csv != NULL) {return csv->rows_n;}
	return -1;
}

int csv_data_get_cols_n(csv_data_t *csv)
{
	if (csv != NULL) {return csv->cols_n;}
	return -1;
}

int csv_data_set_cols_n(csv_data_t *csv, int n)
{
	if (csv != NULL) {
		csv->cols_n = n;
		return csv->cols_n;
	}
	return -1;
}

int csv_data_get_col_curr(csv_data_t *csv)
{
	if (csv != NULL) {return csv->col_curr;}
	return -1;
}

int csv_data_set_col_curr(csv_data_t *csv, int c)
{
	if (csv != NULL) {
		csv->col_curr = c;
		return csv->col_curr;
	}
	return -1;
}

hashtable_t **csv_data_get_columns(csv_data_t *csv)
{
	if (csv != NULL) {return csv->columns;}
	return NULL;
}

hashtable_t **csv_data_new_columns(csv_data_t *csv)
{
	if (csv != NULL) {
		csv->columns = calloc(csv->cols_n, sizeof(hashtable_t*));
		if (csv->columns == NULL) {
			return NULL;
		}
		return csv->columns;
	}
	return NULL;
}

hashtable_t **csv_data_get_column_to_nulls(csv_data_t *csv)
{
	if (csv != NULL) {return csv->column_to_nulls;}
	return NULL;
}

hashtable_t **csv_data_new_column_to_nulls(csv_data_t *csv)
{
	if (csv != NULL) {
		csv->column_to_nulls = calloc(csv->cols_n, sizeof(hashtable_t*));
		if (csv->column_to_nulls == NULL) {
			return NULL;
		}
		return csv->column_to_nulls;
	}
	return NULL;
}

char **csv_data_get_nulls(csv_data_t *csv)
{
	if (csv != NULL) {return csv->nulls;}
	return NULL;
}

float **csv_data_avg_probabilities_new(csv_data_t *csv)
{	
	if (csv != NULL) {
		csv->avg_probabilities = calloc(csv->cols_n, sizeof(float*));
		if (csv->avg_probabilities == NULL) {
			return NULL;
		}

		for (int i = 0; i < csv->cols_n; i++) {
			float *avg = malloc(sizeof(float));
			int sum = 0;
			hashtable_iterate(*(csv->columns+i), &sum, count_unique_rows);
			*(avg) = (float)1 / (float)sum;
			csv->avg_probabilities[i] = avg;
		}

		return csv->avg_probabilities;
	}
	else {
		return NULL;
	}
}

/* To count number of items in hashtable; used as func in hashtable_iterate
 * @param arg running total of items
 * @param key word
 * @param val dummy item
 */
static void count_unique_rows(void *arg, const char *key, void *val)
{
	if (key != NULL && val != NULL) {
		++*((int *)arg);
	}
}

float **csv_data_get_avg_probabilities(csv_data_t *csv)
{
	if (csv != NULL) {return csv->avg_probabilities;}
	return NULL;
}
