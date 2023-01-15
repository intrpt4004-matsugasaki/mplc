#ifndef TypeAllocator_h
#define TypeAllocator_h

#include "Parser.h"
#include "SemanticAnalyzer.h"

static void allocate_type_in_statement(program_t *program, REF_SCOPE scope, statement_t *s);
extern void allocate_type(program_t *program);

#endif
