#include <stdio.h>
#include <stdlib.h>
#include "csv_data.h"
#include "hashtable.h"

typedef struct csv_data {
	int cols_n; // Num of cols in file
	int col_curr; // Current column # whose field we're reading
	hashtable_t **columns; // Each hashtable in array reps a column, in each column table key is val, val is freq
	hashtable_t *column_to_nulls; // Each key is column, val is char **array of possible null values
	hashtable_t *nulls;
} csv_data_t;

csv_data_t *csv_data_new(hashtable_t *nulls)
{
	if (nulls == NULL) {
		return NULL;
	}

	csv_data_t *new = malloc(sizeof(csv_data_t));
	if (new == NULL) {
		return NULL;
	}
	
	new->cols_n = 0;
	new->col_curr = 0;
	new->columns = NULL;
	new->column_to_nulls = NULL;
	new->nulls = nulls;

	return new;
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

hashtable_t *csv_data_get_column_to_nulls(csv_data_t *csv)
{
	if (csv != NULL) {return csv->column_to_nulls;}
	return NULL;
}

hashtable_t *csv_data_new_column_to_nulls(csv_data_t *csv)
{
	if (csv != NULL) {
		csv->column_to_nulls = hashtable_new(csv->cols_n);
		if (csv->column_to_nulls == NULL) {
			return NULL;
		}
		return csv->column_to_nulls;
	}
	return NULL;
}

hashtable_t *csv_data_get_nulls(csv_data_t *csv)
{
	if (csv != NULL) {return csv->nulls;}
	return NULL;
}
