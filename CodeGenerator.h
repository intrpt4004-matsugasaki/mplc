#ifndef CodeGenerator_h
#define CodeGenerator_h

#include <stdarg.h>
#include "Parser.h"

extern char *get_stem(char *filename);

typedef enum {
	FEED_START,
	FEED_LABEL, FEED_INST, FEED_COMM
} FEED_STEP;

static void LABEL(char *label_format, ...);
static void INSTR(char *opcode, char *operand, ...);
static void COMME(char *comment);

static void generate_code_constant(FILE *fp, constant_t cons);
static void generate_code_variable_indicator(FILE *fp, variable_indicator_t var_idr);
static void generate_code_factor(FILE *fp, factor_t factor);
static void generate_code_term(FILE *fp, term_t term);
static void generate_code_simple_expression(FILE *fp, simple_expression_t simp_expr);
static void generate_code_expression(FILE *fp, expression_t expr);
static void generate_code_statement(FILE *fp, statement_t *stmt);
extern void generate_code(char *filename, program_t program);

#endif
