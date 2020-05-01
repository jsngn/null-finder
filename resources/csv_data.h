/* Struct for keeping track of all info we need about a csv file to find null-equivalent values
 * See more details on what's stored in .c file
 * Josephine Nguyen, April 2020
 */

#ifndef __CSV_DATA_H
#define __CSV_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"

/* Type definition */
typedef struct csv_data csv_data_t;

/* Initialize a new struct
 * @param nulls ptr to an ALREADY POPULATED char array of pre-determined null words
 * @param rows number of rows in file, should be supplied by user
 * @return ptr to new struct or NULL if error
 */
csv_data_t *csv_data_new(char **nulls, int rows);

/* Get number of rows in file from struct
 * @param csv struct of interest
 * @return number of rows or -1 if error
 */
int csv_data_get_rows_n(csv_data_t *csv);

/* Get number of cols in file from struct
 * @param csv struct of interest
 * @return number of cols or -1 if error
 */
int csv_data_get_cols_n(csv_data_t *csv);

/* Set number of cols in file
 * @param csv struct to modify
 * @param n new cols #
 * @return n if success, -1 otherwise
 */
int csv_data_set_cols_n(csv_data_t *csv, int n);

/* Get current col we're processing (this can be for anything--while reading csv, iterating through cols hashtable, etc.)
 * @param csv struct of interest
 * @return current col # or -1 if error
 */
int csv_data_get_col_curr(csv_data_t *csv);

/* Set current col we're processing (updating as we're iterating through cols)
 * @param csv struct to modify
 * @param c current col #
 * @return c or -1 if error
 */
int csv_data_set_col_curr(csv_data_t *csv, int c);

/* Get columns hashtable array
 * @param csv struct of interest
 * @return ptr to columns, NULL if error
 */
hashtable_t **csv_data_get_columns(csv_data_t *csv);

/* Initialize columns hashtable array in struct
 * @param csv struct of interest
 */
hashtable_t **csv_data_new_columns(csv_data_t *csv);

/* Get columns hashtable array
 * @param csv struct of interest
 * @return ptr to columns, NULL if error
 */
hashtable_t **csv_data_get_column_to_nulls(csv_data_t *csv);

/* Initialize column_to_nulls hashtable array in struct
 * @param csv struct of interest
 */
hashtable_t **csv_data_new_column_to_nulls(csv_data_t *csv);

/* Get null words string array
 * @param csv struct of interest
 * @return string array or NULL if error
 */
char **csv_data_get_nulls(csv_data_t *csv);

/* Initialize new float array holding average probabilities of words in a col
 * @param csv struct of interest
 * @return ptr to array or NULL if error
 */
float **csv_data_avg_probabilities_new(csv_data_t *csv);

/* Get float array holding average probabilities of words in a col
 * @param csv struct of interest
 * @return ptr to array of NULL if error
 */
float **csv_data_get_avg_probabilities(csv_data_t *csv);

#endif
