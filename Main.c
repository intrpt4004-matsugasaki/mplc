/********************************************
 * mplc: mpl compiler                       *
 *                                          *
 * $ gcc -std=c99 -pedantic Debug.c         *
 *     Main.c Scanner.c Parser.c            *
 *       SemanticAnalyzer.c                 *
 *         TypeAllocator.c CodeGenerator.c  *
 *           -o mpplc                       *
 *                                          *
 *                     gcc version: 12.2.0  *
 ********************************************/

#include <stdio.h>
#include <stdlib.h>

#include "Scanner.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "TypeAllocator.h"
//#include "CodeOptimizer.h"
#include "CodeGenerator.h"

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
	name_analyze(p);
	allocate_type(&p);
	type_analyze(p);
	print_xref_table(p);
	//optimize_code(&p);
	generate_code(assign_csl_ext(get_stem(argv[1])), p);
}

/* TODO */
/*
 * impl: Debug.c
 * BUG FIX: type checker (and type allocator?)
 * expression_t; yuusendo and zentai tositeno type_t wo kouryo sita data structure ni.
 *
 * impl: Code Generator
 *
 * Scanner ~> Tokenizer: file -> [token_t]
 * Parser: [token_t] -> program_t
 *
 * unite error()
 */
