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
		printf("[ERROR] Compiler: File %s can not open.\n", argv[1]);
		exit(-1);
	}

	if (parse_program() == ERROR) {
		/* ERROR */
		printf("[ERROR] Compiler: Syntax parsing failed.\n");
		exit(-1);
	}

	if (semantic_analyze() < 0) {
		/* ERROR */
		printf("[ERROR] Compiler: Semantic analysis failed.\n");
		exit(-1);
	}

	/*if (generate_code("a.out") < 0) {
		*//* ERROR *//*
		printf("[ERROR] Compiler: Code generation failed.\n");
		exit(-1);
	}*/
}
