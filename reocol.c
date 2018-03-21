#include <stdio.h>

#include "reocol.h"

void reorder_row(char *r, int n){
	char buf[n];
	for(int c = 0; c < n; c++){
		buf[(n/3) * (c%3) + (c/3)] = r[c];
	}
	for(int c = 0; c < n; c++){
		r[c] = buf[c];
	}
}

void reorder_row_rev(char *r, int n){
	char buf[n];
	for(int c = 0; c < n; c++){
		buf[c] = r[(n/3) * (c%3) + (c/3)];
	}
	for(int c = 0; c < n; c++){
		r[c] = buf[c];
	}
}

void reorder_cols(char *data, int n, int r){
	for(int i = 0; i < r; i++){
		reorder_row(&data[i*n], n);
	}
}

void reorder_cols_rev(char *data, int n, int r){
	for(int i = 0; i < r; i++){
		reorder_row_rev(&data[i*n], n);
	}
}
