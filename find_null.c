/* Program to take in a CSV file and give back (potential) NULL values
 * Useful for uncleaned datasets with obscure, hidden NULL values
 * Josephine Nguyen, April 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "hashtable.h"
#include "csv.h"
#include "csv_data.h"

int validate_args(int argc, char *argv[]);
int read_nulls(char *file, char **null_words);
int read_csv(char *argv[]);
void on_field_read (void *s, size_t len, void *data);
void on_row_read (int c, void *data);
int get_word_count(char *string, size_t len);
void get_occurrence_probability(void *data, const char *key, void *val);
void print_probabilities(void *data, const char *key, void *val);
void find_nulls_by_probabilities(void *data, const char *key, void *val);
void print_nulls(void *data, const char *key, void *val);

/* Validates args, reads CSV and prints possible null-equivalent phrases by column #
 * @param argc # args passed
 * @param argv args string array
 * @return exit status
 */
int main(int argc, char *argv[])
{
	int stat;
	if ((stat = validate_args(argc, argv)) != 0) {
		return stat;
	}
	
	if ((stat = read_csv(argv)) != 0) {
		return stat;
	}

	return 0;
}

/* Validates usage & arguments
 * @param argc as in main
 * @param argv as in main
 * @return exit status
 */
int validate_args(int argc, char *argv[])
{
	if (argc != 4) {
		fprintf(stderr, "Usage: ./find_null null_file csv_file rows_num\n");
		return 1;
	}

	return 0;
}

/* Reads words in file w/ pre-defined null words, puts into char array
 * @param file path to null file
 * @param null_words string array we fill up with null words
 * @return exit status
 */
int read_nulls(char *file, char **null_words)
{
	if (file != NULL && null_words != NULL) {
		FILE *fp; // File we read from
		int c; // Each char we read from file, one at a time
		char *word = calloc(50, sizeof(char));
		*word = '\0'; // Just to be safe
		int words_idx = 0; // Index in null_words to insert into	

		if ((fp = fopen(file, "r")) == NULL) {
			return 4;
		}
		
		while ((c = fgetc(fp)) != EOF) {
			if (c == '\n') { // We've got a null word
				if (strlen(word) > 0) { // In case multiple whitespaces btwn words
					*(null_words + words_idx) = word;
					words_idx++;
					word = calloc(50, sizeof(char));
					if (word == NULL) {
						fclose(fp);
						return 4;
					}
					*word = '\0';
				}
			}
			else { // Still building onto a word
				int len_so_far = strlen(word);
				char n = (char) c;
				// Concatenate to built word w/o strange char at end of each char as with strcat
				*(word+len_so_far) = n;
				*(word+len_so_far+1) = '\0';
			}
		}
		free(word); // For extra word allocated just before EOF
		fclose(fp);
		return 0;
		}
	else {
		return 2;
	}
}

/* Reads, processes CSV, prints potential null words by column #
 * Calls read_nulls to get array of defined null-equivalent words first
 * @param argv same as main
 * @return exit status
 */
int read_csv(char *argv[])
{
	char **null_words; // Array to fill up w/ defined null words
	const int null_num = 16;
	FILE *fp; // CSV
	char line[5120]; // Buffer for each line read from CSV file
	struct csv_parser csv_obj; // Parser for csvlib
	csv_data_t *csv_info; // Holds hashtables of values in columns, null values in columns, other info about csv
	size_t bytes_read; // For csvlib, make sure no error in read
	float **avg_probabilities; // Array of floats representing avg probability w/ which unique words show up in each col
	hashtable_t **column_to_nulls; // Hashtable array of null words in every column (every item is hashtable of present null words) 
	hashtable_t **columns; // Hashtable of array of unique words in every column (every item is hashtable of word as key, probability at which they occur as value)
	
	// Read from file of pre-defined null words
	null_words = calloc(null_num, sizeof(char*));
	if (null_words == NULL) {
		return 4;
	}
	read_nulls(argv[1], null_words);
	
	/* Read from csv */
	
	// Get rows num provided by user, add to struct of csv info
	char* rem;
	int rows = (int)strtol(argv[3], &rem, 10);
	csv_info = csv_data_new(null_words, rows);
	if (csv_info == NULL) {
		return 4;
	}
	
	// Initialize csv parser
	if (csv_init(&csv_obj, 0) != 0) {
		return 4;
	}
	
	// Open file, read line by line, calling callback functions w/ every field & row read
	// to populate hashtables of words in each column & null words in each column
	if ((fp = fopen(argv[2], "r")) == NULL) {
		return 4;
	}
	while ((bytes_read = fread(line, sizeof(char), 5120, fp)) > 0) {
		if (csv_parse(&csv_obj, line, bytes_read, on_field_read, on_row_read, csv_info) != bytes_read) {
			fprintf(stderr, "Error parsing.\n");
			return 4;
		}
	}
	
	// Calculate avg probability w/ which words occur in each col
	avg_probabilities = csv_data_avg_probabilities_new(csv_info);
	/*for (int i = 0; i < csv_data_get_cols_n(csv_info); i++) {
		printf("%d %f\n", i, **(avg_probabilities+i));
	}*/
	
	// Get ref to hashtable arrays
	column_to_nulls = csv_data_get_column_to_nulls(csv_info);
	if (column_to_nulls == NULL) {
		return 4;
	}
	columns = csv_data_get_columns(csv_info);
	if (columns == NULL) {
		return 4;
	}

	/*for (int i = 0; i < csv_data_get_cols_n(csv_info); i++) {
		hashtable_print(*(columns+i));
	}*/
	
	// Loop through all hashtables in columns to divide freq of a unique word by total rows to get probability
	// Then add any new null words detected by probability to column_to_nulls
	for (int i = 0; i < csv_data_get_cols_n(csv_info); i++) {
		csv_data_set_col_curr(csv_info, i); // So in find_nulls_by_probabilities, know which array item to insert
		hashtable_iterate(*(columns+i), &rows, get_occurrence_probability);
		hashtable_iterate(*(columns+i), csv_info, find_nulls_by_probabilities);
		//hashtable_iterate(*(columns+i), NULL, print_probabilities);
	}
	
	// Print results
	for (int i = 0; i < csv_data_get_cols_n(csv_info); i++) {
		printf("COLUMN %d: \n", i+1);
		hashtable_iterate(*(column_to_nulls+i), NULL, print_nulls);	
		printf("\n");
	}
	
	// Clean up
	csv_fini(&csv_obj, on_field_read, on_row_read, csv_info);
	csv_free(&csv_obj);
	fclose(fp);
	for (int i = 0; i < null_num; i++) {
		free(null_words[i]);
	}
	free(null_words);
	for (int i = 0; i < csv_data_get_cols_n(csv_info); i++) {
		hashtable_free(*(column_to_nulls+i));
	}
	free(column_to_nulls);
	for (int i = 0; i < csv_data_get_cols_n(csv_info); i++) {
		hashtable_free(*(columns+i));
	}
	for (int i = 0; i < csv_data_get_cols_n(csv_info); i++) {
		free(*(avg_probabilities+i));
	}
	free(avg_probabilities);
	free(columns);
	free(csv_info);

	return 0;
}

/* Prints a single null word in a hashtable in an iterate function
 * @param data expect NULL
 * @param key the null word
 * @param val expect dummy item, not NULL but meaningless
 */
void print_nulls(void *data, const char *key, void *val)
{
	if (key != NULL) {
		printf("%s, ", key);
	}
}

/* Detects null words, determined as words w/ low enough probabilities of showing up in that col
 * then adds words to column_to_nulls
 * @param data csv_data_t ptr, holding the 2 important hashtable arrays
 * @param key current word we're determining whether null
 * @val word's probability
 */
void find_nulls_by_probabilities(void *data, const char *key, void *val)
{
	if (data != NULL && key != NULL && val != NULL) {
		csv_data_t *info = (csv_data_t *)data;
		hashtable_t *column_nulls = *(csv_data_get_column_to_nulls(info) + csv_data_get_col_curr(info));
		float *avg = *(csv_data_get_avg_probabilities(info) + csv_data_get_col_curr(info));
		float *prob = (float *)val;
		char *field_cp = (char *)key;
		
		// Probability sufficiently less than avg probability in col, and word isn't too long in terms of length & word #
		if (((*avg < 0.5 && *prob <= *avg * 0.02) || (*avg >= 0.5 && *prob < *avg * 0.02)) && get_word_count(field_cp, strlen(field_cp)) <= 3 && strlen(field_cp) < 10)  {
			/*if (csv_data_get_col_curr(info)+1 == 11) {
				printf("key: %s prob: %f avg: %f\n", field_cp, *prob, *avg);
			}*/

			char *dummy = malloc(sizeof(char)); // Dummy value item for inserting new null word detected
			char *key_cp = calloc(strlen(key)+1, sizeof(char));
			strcpy(key_cp, key);
			if (hashtable_insert(column_nulls, key_cp, dummy) != 0) { // Clean up if can't insert e.g. already detected w/ pre-defined null words
				free(dummy);
				free(key_cp);
			}
		}
	}
}

/* Print probability of each word in col, for testing
 * @param data expect NULL
 * @param key word
 * @param val probability
 */
void print_probabilities(void *data, const char *key, void *val)
{
	if (key != NULL && val != NULL) {
		printf("%s : %f\n", (char *)key, *((float *)val));
	}
}

/* Calculate occurrence probability of a word in a col
 * @param data total rows in csv
 * @param key word
 * @param val initially frequency, we change to probability in function
 */
void get_occurrence_probability(void *data, const char *key, void *val)
{
	if (data != NULL && val != NULL) {
		int *total = (int *)data;
		float *prob = (float *)val;
		*(prob) = *(prob) / (float)(*(total));
	}
}

/* Callback function every time we finish reading a field
 * @param s field string
 * @param len length of field string
 * @data csv_data_t* ptr, holding other csv info -- update data structures in struct here
 */
void on_field_read (void *s, size_t len, void *data)
{
	csv_data_t *info = (csv_data_t*)data;
	csv_data_set_col_curr(info, csv_data_get_col_curr(info)+1); // For reference later, we know which column we're on
	
	if (csv_data_get_cols_n(info) == 0) {return;} // The rest is for 2nd+ rows
	
	char *s_char = (char *)s; // For rebuilding s
	char *field = calloc((len+1), sizeof(char)); // s isn't completely sanitized, so just rebuild our own s
	int len_so_far = 0; // For rebuilding s
	char **null_words; // Get from csv info
	hashtable_t **column_to_nulls; // Get from csv info
	hashtable_t **columns; // Get from csv info
	
	// Build s as field
	while (len) {
		len_so_far = strlen(field);
		*(field+len_so_far) = *(s_char);
		*(field+len_so_far+1) = '\0';
		s_char++;
		len--;
	}
	
	char *field_cp = calloc(strlen(field)+1, sizeof(char)); // Potentially insert into both columns & column_to_nulls so need 2 separate refs
	strcpy(field_cp, field);

	null_words = csv_data_get_nulls(info);
	column_to_nulls = csv_data_get_column_to_nulls(info);
	columns = csv_data_get_columns(info);
	
	/* Insert into columns */
	float *count = malloc(sizeof(int)); // Item which we insert in, if never seen word before
	*count = 1;
	int stat = 0; // Track status see if successful insert
	if ((stat = hashtable_insert(*(columns+csv_data_get_col_curr(info)-1), field, count)) == 5) { // Repeated item
		free(count);
		count = NULL;
		count = (hashtable_find(*(columns+csv_data_get_col_curr(info)-1), field));
		if (count != NULL) {
			++*(count); // Increment existing freq
		}
		free(field);
	}
	else if (stat == 4) { // Some malloc error, just clean up
		free(field);
		free(count);
	}
	
	/* Insert into column_to_nulls */
	for (int i = 0; i < 16; i++) {
		char *curr = *(null_words+i); // Current pre-defined null word
		// Current null word is substring of field, word is short enough (word # and string length) absolutely & relatively compared to null word
		if (strstr(field_cp, curr) != NULL && get_word_count(field_cp, strlen(field_cp)) <= 3 && strlen(field_cp) < 10 && strlen(field_cp) < (strlen(curr) * 2)) {
			char *dummy = malloc(sizeof(char));
		
			if (hashtable_insert(*(column_to_nulls+csv_data_get_col_curr(info)-1), field_cp, dummy) == 0) {
				return;
			}
			else { // Clean up if unsuccessful
				free(dummy);
			}
		}
	}

	// Alternatively, if field is empty, this may be considered null, represented by <empty>
	if (strlen(field_cp) == 0) {
		char *empty = calloc(8, sizeof(char));
		strcpy(empty, "<empty>");
		char *dummy = malloc(sizeof(char));
		if (hashtable_insert(*(column_to_nulls+csv_data_get_col_curr(info)-1), empty, dummy) != 0) {
			free(empty);
			free(dummy);
		}
	}
	free(field_cp);
}

/* Callback function every time we finish reading a row
 * @param c current char processed by csvlib
 * @param data csv_data_t*, holds data about csv
 */
void on_row_read (int c, void *data)
{ 
	hashtable_t **column_to_nulls;
	hashtable_t **columns;
	csv_data_t *info = (csv_data_t*)data;

	if (csv_data_get_cols_n(info) == 0) { // Only true after read first row
		if (csv_data_set_cols_n(info, csv_data_get_col_curr(info)) == csv_data_get_col_curr(info)) { // Set total col number in csv info struct
			// Set up columns & column_to_nulls; both initially empty
			if ((columns = csv_data_new_columns(info)) == NULL) {
				fprintf(stderr, "Malloc error\n");
				return;
			}

			for (int i = 0; i < csv_data_get_cols_n(info); i++) {
				if ((*(columns+i) = hashtable_new(csv_data_get_rows_n(info) * 2)) == NULL) {
					fprintf(stderr, "Malloc error for columns\n");
					return;
				}
			}

			if ((column_to_nulls = csv_data_new_column_to_nulls(info)) == NULL) {
				fprintf(stderr, "Malloc error\n");
				return;
			}

			for (int i = 0; i < csv_data_get_cols_n(info); i++) {
				if ((*(column_to_nulls+i) = hashtable_new(csv_data_get_rows_n(info) * 2)) == NULL) {
					fprintf(stderr, "Malloc error for column_to_nulls\n");
					return;
				}
			}

		}
	}
	csv_data_set_col_curr(info, 0); // Reset current column processing to 0 every time finish a row
}

/* Calculates how many words in a string based on number of whitespace
 * @param string word
 * @param len length of word
 * @return number of words
 */
int get_word_count(char *string, size_t len)
{
	int c = 1; // Account for 1st word w/ no preceding whitespace
	for (int i = 0; i < len; i++) {
		if (isspace(*(string+i)) != 0) {
			c++;
		}
	}
	return c;
}
