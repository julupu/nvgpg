#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "reocol.h"
#include "playfair.h"

//#define DEBUG
#define INIT_SIZE 4096
#define ENCRYPT 1
#define DECRYPT 0


/*
* returns 1 if opt is set
* or 0 if opt is not set
*/
static int opt_is_set(char *opt, int argc, char **argv){
	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], opt) == 0) return 1;
	}
	return 0;
}

/*
* Returns position of parameter in argument array
* or -1 if opt is set but parameter missing
* or 0 if opt is not set at all
*/
static int get_param(char *opt, int argc, char **argv){
	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], opt) == 0){
			if(i == argc-1) return -1;
			if(strncmp(argv[i+1], "--", 2) == 0) return -1;
			else return i+1;
		}
	}
	return 0;
}

/* Transpose matrix */
char* transpose(char *data, int length, int n){
	char *trans = malloc(length * 2 * sizeof(char));
	if(!trans) perror("transpose malloc");
	//printf("transpose length %d chars, n is %d and lincnt is %d\n", length, n, length/n);

	int write_pos = 0;
	for(int i = 0; i < n; i++){
		for(int j = 0; j < length/n; j++){
			char c = data[j*n+i];
			trans[write_pos] = c;
			write_pos++;
		}
	}
	trans[write_pos] = '\0';
	//printf("transposed %d chars\n", write_pos);
	return trans;
}

char* transpose_rev(char *data, int length, int n){
	char *trans = malloc(length * sizeof(char));
	if(!trans) perror("transpose_rev malloc");
	//printf("transpose length %d chars, n is %d and lincnt is %d\n", length, n, length/n);

	int read_pos = 0;
	for(int j = 0; j < n; j++){
		for(int i = 0; i < length/n; i++){
			char c = data[read_pos];
			trans[i*n+j] = c;
			read_pos++;
		}
	}
	trans[length] = '\0';
	//printf("transposed %d chars\n", read_pos);
	return trans;
}


static void dbg(char *str){
	#ifdef DEBUG
	printf("%s\n", str);
	#endif
}

static void err(char *str){
	fprintf(stderr, "%s\n", str);
	exit(1);
}
static void print_data(char *data, int pos, int n){
	for(int i = 0; i < pos; i++){
		if(i>0 && i%n == 0) printf("\n");
		printf("%c",data[i]);
	}
	printf("\n");
}

char* handle_input(int mode, char *data, int cur_pos, int n){
	char tmpbuf;
	char *trans = 0;
	if(mode == ENCRYPT){
		//dbg("Original");
		//print_data(data, cur_pos, n);
		//printf("\n");

		for(int i = 0; i < cur_pos; ){
			if(i >= cur_pos -1){
				tmpbuf = pf_lookup(data[i], 'X');
				data[i+1] = pf_lookup('X', data[i]);
				cur_pos++;
				while(cur_pos % n != 0){
					data[cur_pos] = 'X';
					//printf("11cur_pos: %d -%c-\n", cur_pos, data[cur_pos]);
					cur_pos++;
				}
			}else{
				tmpbuf = pf_lookup(data[i], data[i+1]);
				data[i+1] = pf_lookup(data[i+1], data[i]);
			}
			
			data[i] = tmpbuf;
			if(i == cur_pos -1){
				//printf("end of buffer, writing %c to %d and %c to %d\n", data[i], i, data[i+1], i+1);
			}
			i+=2;
			//printf("Two chars looked up %d\n", i);
		}
		//dbg("Playfair");
		//print_data(data, cur_pos, n);
		//printf("\n");

		reorder_cols(data, n, cur_pos/n);
		//dbg("Cols reordered");
		//print_data(data, cur_pos, n);
		//printf("\n");

		trans = transpose(data, cur_pos+1, n);
		//dbg("Transposed");
		//print_data(trans, strlen(trans), n);
	}else if(mode == DECRYPT){
		dbg("Original");
		//print_data(data, cur_pos, n);
		//printf("strlen: %d, cur_pos: %d\n", (int) strlen(data),cur_pos);
		//printf("Transposing..\n");

		trans = transpose_rev(data, cur_pos+1, n);
		dbg("Transposed");
		//print_data(trans, strlen(trans), n);
		//printf("strlen: %d, cur_pos: %d\n", (int) strlen(data),cur_pos);
		//printf("\n");

		//printf("Reordering..\n");
		reorder_cols_rev(trans, n, strlen(trans)/n);
		dbg("Cols reordered");
		//print_data(trans, strlen(trans), n);
		//printf("strlen: %d, cur_pos: %d\n", (int) strlen(data),cur_pos);
		//printf("\n");

		//printf("Playfair lookup..\n");
		for(int i = 0; i < strlen(trans); ){
			if(i == strlen(trans)-1){
				tmpbuf = pf_lookup_rev(trans[i], 'X');
				trans[i+1] = pf_lookup_rev('X', trans[i]);
			}else{
				tmpbuf = pf_lookup_rev(trans[i], trans[i+1]);
				trans[i+1] = pf_lookup_rev(trans[i+1], trans[i]);
			}
			trans[i] = tmpbuf;
			if(i == strlen(trans)-1){
				//printf("end of buffer, writing %c to %d and %c to %d\n", trans[i], i, trans[i+1], i+1);
			}
			i+=2;
			//printf("Two chars looked up %d\n", i);
		}
		dbg("Playfair");
		//printf("strlen: %d, cur_pos: %d\n", (int) strlen(data),cur_pos);
		//print_data(trans, strlen(trans), n);
	}

	return trans;


}

void output(char *data, int n, int pos, FILE *fp_out){
	//int pos = strlen(data);
	//fputc('\n',fp_out);
	fputc('\n',fp_out);
	//printf("writing %d chars\n", pos);
	for(int i = 0; i < pos; i++){
		if(i>0 && i%n == 0) fputc('\n', fp_out);
		fputc(data[i],fp_out);
	}
	//fputc('\n',fp_out);
}

int main(int argc, char **argv){
	int mode; /* true for encryption, false for decryption */
	char *key = 0;
	char *file = 0;
	int tmp = 0;
	int n = 0;


	/*
	* Check for encryption or decryption mode
	* either by binary-name or option
	*/
	if((strstr(argv[0], "nvgpg-decrypt") != 0) ||
		opt_is_set("--decrypt", argc, argv)){
		mode = DECRYPT;
		dbg("Mode set to decrypt");
	}else if((strstr(argv[0], "nvgpg-encrypt") != 0) || 
		opt_is_set("--encrypt", argc, argv)){
		mode = ENCRYPT;
		dbg("Mode set to encrypt");
	}else{
		err("No mode set");
	}

	/*
	* Check if key was specified	
	*/
	if(opt_is_set("--key", argc, argv)){
		tmp = get_param("--key", argc, argv);
		if(tmp > 0){
			key = argv[tmp];
			dbg("Key specified");
		}else if(tmp == -1){
			err("Key opt set, but no key specified");
		}
	}else{
		err("No key specified");
	}

	/*
	* Check key validity, filter duplicates and transform to uppercase
	*/
	int keylen = strlen(key);
	char *trans_key = malloc(keylen*sizeof(char));
	if(!trans_key){
		perror("malloc trans_key");
		exit(1);
	}
	for(int i = 0; i < keylen; i++){
		trans_key[i] = '\0';
	}
	int pos = 0;
	for(int i = 0; i < keylen; i++){
		if(key[i] < 65 || (key[i] > 90 && key[i] < 97) || key[i] > 122){
			err("Invalid key specified, characters need to be in A-Z, a-z");
		}
		if(key[i] > 96 && key[i] < 123){
			key[i] = key[i]-32;
		}
		if(in_key(key[i], trans_key)) continue;
		trans_key[pos] = key[i];
		pos++;
	}
	key = trans_key;

	/*
	* Check if n was specified	
	*/
	if(opt_is_set("--n", argc, argv)){
		tmp = get_param("--n", argc, argv);
		if(tmp > 0){
			n = atoi(argv[tmp]);
			dbg("n specified");
		}else if(tmp == -1){
			err("n opt set, but no value specified");
		}
	}else{
		err("n not specified");
	}
	if(n == 0 || n%3 != 0) err("Invalid n");

	/*
	* Check if input-file was specified	
	*/
	tmp = 0;
	FILE *fp = stdin;
	if(opt_is_set("--in", argc, argv)){
		tmp = get_param("--in", argc, argv);
		if(tmp > 0){
			file = argv[tmp];
			dbg("File specified");
			fp = fopen(file, "r");
			if(!fp){
				perror("fopen");
				exit(1);
			}
		}else if(tmp == -1){
			err("File opt set, but no file specified");
		}
	}else{
		dbg("No file specified, using stdin");
	}
	/*
	* Check if output-file was specified	
	*/
	tmp = 0;
	FILE *fp_out = stdout;
	if(opt_is_set("--out", argc, argv)){
		tmp = get_param("--out", argc, argv);
		if(tmp > 0){
			file = argv[tmp];
			dbg("File specified");
			fp_out = fopen(file, "w");
			if(!fp_out){
				perror("fopen");
				exit(1);
			}
		}else if(tmp == -1){
			err("File opt set, but no file specified");
		}
	}else{
		dbg("No file specified, using stdout");
	}

	/*
	* Read input from fp (either file, if specified, or stdin)
	*/
	char *data = malloc(n * INIT_SIZE * sizeof(char));
	if(!data) perror("data malloc");

	int cur_pos = 0;
	int cur_size = n * INIT_SIZE;

	pf_init(key);
	//printf("\n");
	char c;
	char prev = '\0';
	int handle_counter = 0;
	while((c = fgetc(fp)) != EOF){
		/* Skip special characters */
		if(c < 65 || c > 122) continue;
		if(c > 90 && c < 97) continue;
		/* Transform to capital letters */
		if(c > 96) c -= 32;
		if(c == 'J') c = 'I';

		if(mode == ENCRYPT && cur_pos > 0 && data[cur_pos-1] == c){
			data[cur_pos] = 'X';
			cur_pos++;
			//printf("1inc cur_pos to %d\n", cur_pos);
		}

		if(mode == ENCRYPT && cur_pos == 0 && prev == c){
			data[cur_pos] = 'X';
			cur_pos++;
		}

		if(cur_pos == 1000 * n){
			char *res = handle_input(mode, data, cur_pos, n);
			//printf("X input handled, %d\n", handle_counter);
			handle_counter++;
			output(res, n, cur_pos, fp_out);
			cur_pos = 0;
			//printf("reset cur_pos to %d\n", cur_pos);
		}

		data[cur_pos] = c;
		cur_pos++;
		//printf("2inc cur_pos to %d\n", cur_pos);

		if(cur_pos == 1000 * n){
			char *res = handle_input(mode, data, cur_pos, n);
			//printf("normal input handled, %d\n", handle_counter);
			handle_counter++;
			output(res, n, cur_pos, fp_out);
			cur_pos = 0;
			//printf("reset cur_pos to %d\n", cur_pos);
		}

		if(cur_pos == cur_size-1){
			dbg("Realloc");
			//printf("Realloc\n");
			cur_size *= 2;
			data = realloc(data, cur_size);
			if(data == 0) err("Realloc error");
		}
		prev = c;


	}
	//printf("1cur_pos: %d -%c-\n", cur_pos, data[cur_pos-1]);

	if(cur_pos > 0){
		/*
		*  Fill last line to the end
		*/
		while(cur_pos % n != 0){
			data[cur_pos] = 'X';
			//printf("2cur_pos: %d -%c-\n", cur_pos, data[cur_pos]);
			cur_pos++;
		}
		data[cur_pos] = '\0';
		//printf("3cur_pos: %d -%c-\n", cur_pos, data[cur_pos]);

		//printf("strlen raw: %d\n", (int) strlen(data));
		//print_data(data, cur_pos, n);
		char *res = handle_input(mode, data, cur_pos, n);
		//printf("strlen res: %d\n", (int) strlen(res));
		output(res, n, strlen(res), fp_out);
	}

	//printf("Key: %s; File: %s; Mode: %d; n: %d\n", key, file, mode, n);
	return 0;
}
