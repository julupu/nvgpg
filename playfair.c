#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "playfair.h"

static char *matrix;
static char *lookup_table;
static char *lookup_table_rev;

static void create_lookup_table();
static void create_lookup_table_rev();
static int get_row(char c);
static int get_col(char c);
static char get_char_pos(int row, int col);
static char get_char(char a, char b);
static char get_char_rev(char a, char b);

void pf_init(char *key){
	int keylen = strlen(key);
	//printf("key: %s, strlen(key): %d\n", key, keylen);

	matrix = malloc(25 * sizeof(char));
	if(!matrix){
		perror("malloc playfair matrix");
		exit(1);
	}

	lookup_table = malloc(26 * 26 * sizeof(char));
	if(!lookup_table){
		perror("malloc playfair lookup_table");
		exit(1);
	}


	lookup_table_rev = malloc(26 * 26 * sizeof(char));
	if(!lookup_table_rev){
		perror("malloc playfair lookup_table_rev");
		exit(1);
	}

	for(int i = 0; i < keylen; i++){
		matrix[i] = key[i];
	}

	char c = 'A';
	for(int i = keylen; c <= 'Z'; ){
		if(!in_key(c, key) && c != 'J'){
			matrix[i] = c;
			//printf("%c not yet in matrix, adding at %d\n", c, i);
			i++;
		}
		c++;
	}
	create_lookup_table();
	create_lookup_table_rev();

	/* Debug output */
	/*
	printf("playfair matrix:\n");
	for(int i = 0; i < 25; i++){
		if(i%5 == 0) printf("\n");
		printf("%c ", matrix[i]);
	}
	printf("\n");

	*/

	/* Debug output */
	/*
	printf("playfair lookup_table:\n");
	for(int i = 0; i < 26*26; i++){
		if(i%26 == 0) printf("\n");
		if(lookup_table_rev[i] == '\0'){
			printf(" ");
			continue;
		}
		printf("%c ", lookup_table[i]);
	}
	printf("\n");
	printf("playfair lookup_table_rev:\n");
	for(int i = 0; i < 26*26; i++){
		if(i%26 == 0) printf("\n");
		if(lookup_table_rev[i] == '\0'){
			printf(" ");
			continue;
		}
		printf("%c ", lookup_table_rev[i]);
	}
	printf("\n");
	*/
}

int in_key(char c, char *key){
	int keylen = strlen(key);
	for(int i = 0; i < keylen; i++){
		if(key[i] == c) return 1;
	}
	return 0;
}

char pf_lookup(char a, char b){
	int offset = 65;
	//printf("lookup %c %c at %d\n", a, b, (a-offset)*25+(b-offset));
	return lookup_table[(a-offset)*26+(b-offset)];
}

char pf_lookup_rev(char a, char b){
	int offset = 65;
	//printf("lookup %c %c at %d %d gives %c\n" , a, b,
	//(a-offset), (b-offset),lookup_table_rev[(a-offset)*25+(b-offset)]);
	return lookup_table_rev[(a-offset)*26+(b-offset)];
}

static void create_lookup_table(){
	int offset = 65;
	for(int i = 0; i < 26; i++){
		for(int j = 0; j < 26; j++){
			if(i == j){
				//if(i == 'X')
				lookup_table[i*26+j] = 'X';
				//else
				//	lookup_table[i*25+j] = '\0';
				//printf("i == j, setting %d %d to 0\n", i, j);
			}else{
				lookup_table[i*26+j] = get_char(i+offset, j+offset);
				//printf("%d %d, res of %c %c is %c\n", i, j, i+offset,
				//j+offset, lookup_table[i*25+j]);
			}
		}
	}
}

static void create_lookup_table_rev(){
	int offset = 65;
	for(int i = 0; i < 26; i++){
		for(int j = 0; j < 26; j++){
			if(i == j){
				//if(i == 'X')
				lookup_table_rev[i*26+j] = 'X';
				//else
				//	lookup_table[i*25+j] = '\0';
				//printf("i == j, setting %d %d to 0\n", i, j);
			}else{
				lookup_table_rev[i*26+j] = get_char_rev(i+offset, j+offset);
				//printf("%d %d, res of %c %c is %c\n", i, j, i+offset,
				//j+offset, lookup_table_rev[i*25+j]);
			}
		}
	}
}

static int get_row(char c){
	if(c == 'J') c = 'I';
	for(int i = 0; i < 26; i++){
		if(matrix[i] == c) return i/5;
	}
	return -1;
}

static int get_col(char c){
	if(c == 'J') c = 'I';
	for(int i = 0; i < 26; i++){
		if(matrix[i] == c) return i%5;
	}
	return -1;
}

static char get_char_pos(int row, int col){
	return matrix[row*5+col];
}

static char get_char(char a, char b){
	if(a == 'J') a = 'I';
	if(b == 'J') b = 'I';

	int row_a = get_row(a);
	int col_a = get_col(a);
	int row_b = get_row(b);
	int col_b = get_col(b);

	if(col_a == col_b){
		return get_char_pos((row_a+1)%5, col_a);
	}
	if(row_a == row_b){
		return get_char_pos(row_a, (col_a+1)%5);
	}

	return get_char_pos(row_a, col_b);
}

static char get_char_rev(char a, char b){
	if(a == 'J') a = 'I';
	if(b == 'J') b = 'I';

	int row_a = get_row(a);
	int col_a = get_col(a);
	int row_b = get_row(b);
	int col_b = get_col(b);

	if(col_a == col_b){
		if(row_a-1 < 0) row_a = 4;
		else row_a-=1;
		return get_char_pos(row_a, col_a);
	}
	if(row_a == row_b){
		if(col_a-1 < 0) col_a = 4;
		else col_a-=1;
		return get_char_pos(row_a, col_a);
	}

	return get_char_pos(row_a, col_b);
}
