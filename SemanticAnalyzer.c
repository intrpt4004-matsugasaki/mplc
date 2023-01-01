#include "SemanticAnalyzer.h"

extern void semantic_analyze(program_t program) {
	print_xref_table(program);
}

static void print_xref_table(program_t program) {
	printf("program: %s\n", program.name);
}
