/*
* Reorder columns in given matrix (array representation) with n columns
* according to the formula: n/3 * c%3 + c/3
* E.g. for 9 columns (0-8) the order should be: 0 3 6 1 4 7 2 5 8
*/

/*
* Reorder a single row r with length n (and reverse)
*/
void reorder_row(char *r, int n);
void reorder_row_rev(char *r, int n);

/*
* Reorder all n columns of data with r rows (and reverse)
*/
void reorder_cols(char *data, int n, int r);
void reorder_cols_rev(char *data, int n, int r);
