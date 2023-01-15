#ifndef SemanticAnalyzer_h
#define SemanticAnalyzer_h

#include "Parser.h"

static void error(char *message);

static void there_is_no_overloaded_name(program_t program);

typedef struct {
	enum { SCOPE_PROGRAM, SCOPE_PROCEDURE } kind;
	char proc_name[MAXSTRSIZE];
} REF_SCOPE;

static void variable_declared(program_t program, REF_SCOPE scope, variable_indicator_t var_idr);
static void procedure_declared(program_t program, char *proc_name);
static void name_declared_in_factor(program_t program, REF_SCOPE scope, factor_t factor);
static void name_declared_in_term(program_t program, REF_SCOPE scope, term_t term);
static void name_declared_in_simple_expression(program_t program, REF_SCOPE scope, simple_expression_t simp_expr);
static void name_declared_in_expression(program_t program, REF_SCOPE scope, expression_t expr);
static void name_declared_in_expressions(program_t program, REF_SCOPE scope, expressions_t *exprs);
static void name_declared_in_variable_indicator(program_t program, REF_SCOPE scope, variable_indicator_t var_idr);
static void name_declared_in_variable_indicators(program_t program, REF_SCOPE scope, variable_indicators_t *var_idrs);
static void name_declared_output_formats(program_t program, REF_SCOPE scope, output_formats_t *fmts);
static void name_declared_in_statement(program_t program, REF_SCOPE scope, statement_t *s);
static void there_is_no_undeclared_name(program_t program);

extern void name_analyze(program_t program);
extern void type_analyze(program_t program);

static char *standard_type_t_to_str(standard_type_t stdtype);
static int print_type_and_get_length(type_t type);
extern void print_xref_table(program_t program);

#endif
