/********************************************
 * mplc: mpl compiler                       *
 *                                          *
 * $ gcc -std=c99 -pedantic                 *
 *        Main.c Scanner.c Parser.c         *
 *             SemanticAnalyzer.c -o cr     *
 *                                          *
 *                     gcc version: 12.2.0  *
 ********************************************/

#include <stdio.h>
#include <stdlib.h>

#include "Scanner.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
//#include "CodeGenerator.h"

int main(const int argc, char *argv[]) {
	if (argc < 2) {
		/* ERROR */
		printf("[ERROR] Compiler: File name is not given.\n");
		exit(-1);
	}

	if (init_scan(argv[1]) < 0) {
		/* ERROR */
		printf("[ERROR] Scanner: File %s can not open.\n", argv[1]);
		exit(-1);
	}

	program_t p = parse_program();
	semantic_analyze(p);

	/*
	char obj[MAXSTRSIZE];
	sprintf(obj, "%s.csl", get_stem(argv[1]));
	generate_code(obj, p);
	*/
}
