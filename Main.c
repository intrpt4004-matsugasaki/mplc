/********************************************
 * mplc: mpl compiler                       *
 *                                          *
 * $ gcc -std=c99 -pedantic Main.c          *
 *                      Scanner.c -o mplc   *
 *                                          *
 *                     gcc version: 12.2.0  *
 ********************************************/

#include <stdio.h>
#include <stdlib.h>

#include "Scanner.h"
#include "TokenCounter.h"

int main(const int argc, char *argv[]) {
	if (argc < 2) {
		printf("File name id not given.\n");
		exit(-1);
	}

	if (init_scan(argv[1]) < 0) {
		printf("File %s can not open.\n", argv[1]);
		exit(-1);
	}

	TokenCounter_t *counter = TokenCounter.New();
	int token_code;
	while ((token_code = scan()) >= 0) {
		//printf("#%s", tokencode_to_str(token_code));
			TokenCounter.AppendToken(counter, Token.New(token_code, string_attr, string_attr, num_attr));
	} end_scan();
	//printf("\n");

	TokenCounter.PrintTable(counter);

	//TokenCounter.Delete(counter);
}
