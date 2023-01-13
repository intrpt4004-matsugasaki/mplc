#ifndef SemanticAnalyzer_h
#define SemanticAnalyzer_h

#include "Scanner.h"
#include "Parser.h"

extern void semantic_analyze(program_t program);

static void print_xref_table(program_t program);

#endif
