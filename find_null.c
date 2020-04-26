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
	char line[2048];
	struct csv_parser csv_obj;
	csv_data_t *csv_info;
	size_t bytes_read;

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

	fclose(fp);
	for (int i = 0; i < 16; i++) {
		printf("%s\n", null_words[i]);
		free(null_words[i]);
	}
	free(null_words);

	return 0;
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

	while (len) {
		len_so_far = strlen(field);
		*(field+len_so_far) = *(s_char);
		*(field+len_so_far+1) = '\0';
		s_char++;
		len--;
	}
	
	//printf("%s\n", field);
	null_words = csv_data_get_nulls(info);
	column_to_nulls = csv_data_get_column_to_nulls(info);

	for (int i = 0; i < 16; i++) {
		char *curr = *(null_words+i);
		if (strstr(field, curr) != NULL && get_word_count(field, strlen(field)) < 3 && strlen(field) < 10 && strlen(field) < (strlen(curr) * 2)) {
			char *dummy = malloc(sizeof(char));
		
			if (hashtable_insert(*(column_to_nulls+csv_data_get_col_curr(info)-1), field, dummy) == 0 ) {
			printf("%s %s\n", field, curr);
			}
		}
	}
	//free(field);
}

void on_row_read (int c, void *data)
{ 
	/*printf("-----------");
	printf("\n");
	printf("%d\n", *((int*)data));
	*((int*)data) = 0;*/
	hashtable_t **column_to_nulls;
	csv_data_t *info = (csv_data_t*)data;

	if (csv_data_get_cols_n(info) == 0) {
		if (csv_data_set_cols_n(info, csv_data_get_col_curr(info)) == csv_data_get_col_curr(info)) {
			/*if (csv_data_new_columns(info) == NULL) {
				fprintf(stderr, "Malloc error\n");
				return;
			}*/
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
		printf("%d\n", csv_data_get_cols_n(info));
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
