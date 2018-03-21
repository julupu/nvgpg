#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	FILE *fp = stdin;
	int n;
	if(argc > 1){
		n = atoi(argv[1]);
	}else{
		n = 120;
	}

	int cnt = 0;
	char c;
	char prev = '\0';
	while((c = fgetc(fp)) != EOF){
		/* Skip special characters */
		if(c < 65 || c > 122) continue;
		if(c > 90 && c < 97) continue;
		/* Transform to capital letters */
		if(c > 96) c -= 32;
		if(c == 'J') c = 'I';
		if(cnt > 0 && cnt%n == 0) fputc('\n', stdout);
		if(prev == c){
			fputc('X', stdout);
			cnt++;
			if(cnt > 0 && cnt%n == 0) fputc('\n', stdout);
		}
		prev = c;
		fputc(c, stdout);
		cnt++;
	}
}
