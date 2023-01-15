#ifndef SemanticAnalyzer_h
#define SemanticAnalyzer_h

#include "Scanner.h"
#include "Parser.h"

static void error(char *message);

static void there_is_no_overloaded_name(program_t program);

typedef struct {
	enum { PROGRAM, PROCEDURE } kind;
	char proc_name[MAXSTRSIZE];
} REF_SCOPE;

static void variable_declared(program_t program, REF_SCOPE scope, char *var_name);
static void procedure_declared(program_t program, char *proc_name);
static void name_declared_in_factor(program_t program, factor_t factor);
static void name_declared_in_term(program_t program, term_t term);
static void name_declared_in_simple_expression(program_t program, simple_expression_t simp_expr);
static void name_declared_in_expression(program_t program, expression_t expr);
static void name_declared_in_expressions(program_t program, expressions_t *exprs);
static void name_declared_in_variable_indicator(program_t program, variable_indicator_t idr);
static void name_declared_in_variable_indicators(program_t program, variable_indicators_t *idrs);
static void name_declared_output_formats(program_t program, output_formats_t *fmts);
static void name_declared_in_statement(program_t program, statement_t *s);
static void there_is_no_undeclared_name(program_t program);

static void check_type();

extern void semantic_analyze(program_t program);

static void print_xref_table(program_t program);

#endif
