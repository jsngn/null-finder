/*
 * Program to take in a CSV file and give back (potential) NULL values
 * Useful for uncleaned datasets with obscure, hidden NULL values
 * Josephine Nguyen, April 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hashtable.h"

int validate_args(int argc, char *argv[]);
int read_nulls(char *file, hashtable_t *null_words);
int read_csv(char *argv[]);
char *tokenize(char *str);

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
	if (argc != 3) {
		fprintf(stderr, "Invalid usage.\n");
		return 1;
	}

	return 0;
}

int read_nulls(char *file, hashtable_t *null_words)
{
	if (file != NULL && null_words != NULL) {
		FILE *fp; // File we read from
		int c; // Each char we read from file, one at a time
		char *word = calloc(50, sizeof(char));
		*word = '\0'; // Just to be safe
		char *dummy = calloc(1, sizeof(char)); // Dummy item to store w/ null vals
	
		if ((fp = fopen(file, "r")) == NULL) {
			return 4;
		}
		
		while ((c = fgetc(fp)) != EOF) {
			if (c == '\n') { // We've got a null word
				if (strlen(word) > 0) { // In case multiple whitespaces btwn words
					hashtable_insert(null_words, word, dummy);
					dummy = calloc(1, sizeof(char));
					if (dummy == NULL) {
						fclose(fp);
						return 4;
					}
					word = calloc(50, sizeof(char));
					if (word == NULL) {
						fclose(fp);
						free(dummy);
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
		free(dummy); // Extra dummy at the end
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
	hashtable_t *null_words;
	FILE *fp; // CSV
	hashtable_t **columns; // Each hashtable in array reps a column, in each column table key is val, val is freq
	hashtable_t *column_to_nulls; // Each key is column, val is char **array of possible null values
	int line_len = 1000; // Starter size
	char *line = malloc(line_len * sizeof(char));
	*line = '\0'; // Safety
	int c;

	// Create hashtable first
	null_words = hashtable_new(21);
	if (null_words == NULL) {
		return 4; 
	}

	read_nulls(argv[1], null_words);

	hashtable_print(null_words);	
	
	// Read from csv
	if ((fp = fopen(argv[2], "r")) == NULL) {
		return 4;
	}

	int col_count = 0;
	while ((c = fgetc(fp)) != EOF) {
		if (c == '\n') { // We've got a full line
			if (strlen(line) > 0) { // In case multiple newlines between lines
				char **tokens; 
				char *token;
				char *curr_line = line;
				int true_len = strlen(line); // tokenize adds \0 so store it before
				int curr_len = 0;
				token = tokenize(curr_line);
				
				while (token != NULL) {
					printf("\n%s", token);
					curr_len += (strlen(token) + 1);
					if (curr_len >= true_len) {
						break;
					}
					curr_line += (strlen(token) + 1);
					token = tokenize(curr_line);
				}
				//if (col_count == 0) { // 1st line/column line
					// split line string by comma, set col_count to length, initialize hashtable**

				//}
				//hashtable_insert(null_words, word, dummy);
				//dummy = calloc(1, sizeof(char));
				//if (dummy == NULL) {
				//	fclose(fp);
				//	return 4;
				//}
				free(line);
				line_len = 1000;
				line = malloc(line_len * sizeof(char));
				if (line == NULL) {
					fclose(fp);
				//	free(dummy);
					return 4;
				}
				*line = '\0';
			}
		}
		else {
			int len_so_far = strlen(line);

			// Expand if needed
			if (len_so_far + 2 > line_len) {
				line_len = line_len * 2;
				line = realloc(line, line_len*sizeof(char));
				if (line == NULL) {
					return 4;
				}
				*(line + len_so_far) = '\0';
			} 

			char n = (char) c;
			// Concatenate to built word w/o strange char at end of each char
			*(line+len_so_far) = n;
			*(line+len_so_far+1) = '\0';
		}
	}


	fclose(fp);
	hashtable_free(null_words);

	return 0;
}

char *tokenize(char *str)
{
	static char *next = NULL;
	if (str) {
		next = str;
	}
	
	//if(!*next) {
		//printf("%s\n", next);
//		return NULL;
//	}
        
	str = next;

	if(*str == '"') {
		str++;
	        next++;
		while(*next && *next != '"') {
			next++;
		}
		if(*next == '"') {
			*next++ = '\0';
		}
	}

	while(*next && *next != ',') {
		next++;
	}

	if(*next) {
		*next++ = '\0';
	}
				       
       	return str;
}
