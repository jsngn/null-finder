#ifndef __CSV_DATA_H
#define __CSV_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"

typedef struct csv_data csv_data_t;

csv_data_t *csv_data_new(char **nulls, int rows);

int csv_data_get_rows_n(csv_data_t *csv);

int csv_data_get_cols_n(csv_data_t *csv);

int csv_data_set_cols_n(csv_data_t *csv, int n);

int csv_data_get_col_curr(csv_data_t *csv);

int csv_data_set_col_curr(csv_data_t *csv, int c);

hashtable_t **csv_data_get_columns(csv_data_t *csv);

hashtable_t **csv_data_new_columns(csv_data_t *csv);

hashtable_t **csv_data_get_column_to_nulls(csv_data_t *csv);

hashtable_t **csv_data_new_column_to_nulls(csv_data_t *csv);

char **csv_data_get_nulls(csv_data_t *csv);

#endif
