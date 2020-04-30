/*
 * Program to take in a CSV file and give back (potential) NULL values
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

int main(int argc, char *argv[])
{
	int stat;
	if ((stat = validate_args(argc, argv)) != 0) {
		return stat;
	}
	
	read_csv(argv);

	return 0;
}

int validate_args(int argc, char *argv[])
{
	if (argc != 4) {
		fprintf(stderr, "Invalid usage.\n");
		return 1;
	}

	return 0;
}

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
			else {
				int len_so_far = strlen(word);
				char n = (char) c;
				// Concatenate to built word w/o strange char at end of each char
				*(word+len_so_far) = n;
				*(word+len_so_far+1) = '\0';
			}
		}
		free(word); // For extra word allocated just before EOF
		fclose(fp);
		return 0;
		}
	else {
		return 4;
	}
}

int read_csv(char *argv[])
{
	char **null_words;
	FILE *fp; // CSV
	char line[5120];
	struct csv_parser csv_obj;
	csv_data_t *csv_info;
	size_t bytes_read;
	float **avg_probabilities;
	
	// Read from file of pre-defined null words
	null_words = calloc(16, sizeof(char*));
	if (null_words == NULL) {
		return 4;
	}
	read_nulls(argv[1], null_words);
	
	// Read from csv
	char* rem;
	int rows = (int)strtol(argv[3], &rem, 10);
	csv_info = csv_data_new(null_words, rows);
	if (csv_info == NULL) {
		return 4;
	}

	if (csv_init(&csv_obj, 0) != 0) {
		return 4;
	}

	if ((fp = fopen(argv[2], "r")) == NULL) {
		return 4;
	}
	while ((bytes_read = fread(line, sizeof(char), 2048, fp)) > 0) {
		if (csv_parse(&csv_obj, line, bytes_read, on_field_read, on_row_read, csv_info) != bytes_read) {
			fprintf(stderr, "Error parsing.\n");
			return 4;
		}
	}
	
	avg_probabilities = csv_data_avg_probabilities_new(csv_info);
	for (int i = 0; i < csv_data_get_cols_n(csv_info); i++) {
		printf("%d %f\n", i, **(avg_probabilities+i));
	}
	hashtable_t **column_to_nulls = csv_data_get_column_to_nulls(csv_info);
	if (column_to_nulls == NULL) {
		return 4;
	}
	hashtable_t **columns = csv_data_get_columns(csv_info);
	if (columns == NULL) {
		return 4;
	}

	/*for (int i = 0; i < csv_data_get_cols_n(csv_info); i++) {
		hashtable_print(*(columns+i));
	}*/
	
	for (int i = 0; i < csv_data_get_cols_n(csv_info); i++) {
		csv_data_set_col_curr(csv_info, i);
		hashtable_iterate(*(columns+i), &rows, get_occurrence_probability);
		hashtable_iterate(*(columns+i), csv_info, find_nulls_by_probabilities);
		//hashtable_iterate(*(columns+i), NULL, print_probabilities);
	}

	for (int i = 0; i < csv_data_get_cols_n(csv_info); i++) {
		printf("COLUMN %d: \n", i+1);
		hashtable_iterate(*(column_to_nulls+i), NULL, print_nulls);	
		printf("\n");
	}
	
	// Clean up
	csv_fini(&csv_obj, on_field_read, on_row_read, csv_info);
	csv_free(&csv_obj);
	fclose(fp);
	for (int i = 0; i < 16; i++) {
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

void print_nulls(void *data, const char *key, void *val)
{
	if (key != NULL) {
		printf("%s, ", key);
	}
}

void find_nulls_by_probabilities(void *data, const char *key, void *val)
{
	if (data != NULL && key != NULL && val != NULL) {
		csv_data_t *info = (csv_data_t *)data;
		hashtable_t *column_nulls = *(csv_data_get_column_to_nulls(info) + csv_data_get_col_curr(info));
		float *avg = *(csv_data_get_avg_probabilities(info) + csv_data_get_col_curr(info));
		float *prob = (float *)val;
		char *field_cp = (char *)key;
		if (((*avg < 0.5 && *prob <= *avg * 0.02) || (*avg >= 0.5 && *prob < *avg * 0.02)) && get_word_count(field_cp, strlen(field_cp)) <= 3 && strlen(field_cp) < 10)  {
			/*if (csv_data_get_col_curr(info)+1 == 11) {
				printf("key: %s prob: %f avg: %f\n", field_cp, *prob, *avg);
			}*/

			char *dummy = malloc(sizeof(char));
			char *key_cp = calloc(strlen(key)+1, sizeof(char));
			strcpy(key_cp, key);	
			if (hashtable_insert(column_nulls, key_cp, dummy) != 0) {
				free(dummy);
				free(key_cp);
			}
		}
	}
}

void print_probabilities(void *data, const char *key, void *val)
{
	if (key != NULL && val != NULL) {
		printf("%s : %f\n", (char *)key, *((float *)val));
	}
}

void get_occurrence_probability(void *data, const char *key, void *val)
{
	if (data != NULL && val != NULL) {
		int *total = (int *)data;
		float *prob = (float *)val;
		*(prob) = *(prob) / (float)(*(total));
	}
}

void on_field_read (void *s, size_t len, void *data)
{
	/*while (len) {
		printf("%c", *((char *)s));
		s++;
		len--;
	}
	printf("\n");
	int *count = (int*)data;
	*count = *count + 1;*/
	csv_data_t *info = (csv_data_t*)data;
	csv_data_set_col_curr(info, csv_data_get_col_curr(info)+1);
	/*hashtable_t *column_to_nulls;
	if ((column_to_nulls = csv_data_get_column_to_nulls(info)) != NULL) {
		
	}*/
	if (csv_data_get_cols_n(info) == 0) {return;} // The rest is for 2nd+ rows
	
	char *s_char = (char *)s;
	char *field = calloc((len+1), sizeof(char));
	int len_so_far = 0;
	char **null_words;
	hashtable_t **column_to_nulls;
	hashtable_t **columns;

	while (len) {
		len_so_far = strlen(field);
		*(field+len_so_far) = *(s_char);
		*(field+len_so_far+1) = '\0';
		s_char++;
		len--;
	}
	
	char *field_cp = calloc(strlen(field)+1, sizeof(char));
	strcpy(field_cp, field);

	//printf("%s\n", field);
	null_words = csv_data_get_nulls(info);
	column_to_nulls = csv_data_get_column_to_nulls(info);
	columns = csv_data_get_columns(info);
	
	float *count = malloc(sizeof(int));
	*count = 1;
	int stat = 0;
	if ((stat = hashtable_insert(*(columns+csv_data_get_col_curr(info)-1), field, count)) == 5) { // Repeated item
		free(count);
		count = NULL;
		count = (hashtable_find(*(columns+csv_data_get_col_curr(info)-1), field));
		if (count != NULL) {
			++*(count);
		}
		free(field);
	//	printf("HERE: %f\n", *count);
	}
	else if (stat == 4) {
		free(field);
		free(count);
		return;
	}
	
	for (int i = 0; i < 16; i++) {
		char *curr = *(null_words+i);
		if (strstr(field_cp, curr) != NULL && get_word_count(field_cp, strlen(field_cp)) <= 3 && strlen(field_cp) < 10 && strlen(field_cp) < (strlen(curr) * 2)) {
			char *dummy = malloc(sizeof(char));
		
			if (hashtable_insert(*(column_to_nulls+csv_data_get_col_curr(info)-1), field_cp, dummy) == 0) {
				return;
			}
			else {
				free(dummy);
			}
		}
	}

	free(field_cp);
}

void on_row_read (int c, void *data)
{ 
	/*printf("-----------");
	printf("\n");
	printf("%d\n", *((int*)data));
	*((int*)data) = 0;*/
	hashtable_t **column_to_nulls;
	hashtable_t **columns;
	csv_data_t *info = (csv_data_t*)data;

	if (csv_data_get_cols_n(info) == 0) {
		if (csv_data_set_cols_n(info, csv_data_get_col_curr(info)) == csv_data_get_col_curr(info)) {
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
	csv_data_set_col_curr(info, 0);
	
}

int get_word_count(char *string, size_t len)
{
	int c = 1;
	for (int i = 0; i < len; i++) {
		if (isspace(*(string+i)) != 0) {
			c++;
		}
	}
	return c;
}
