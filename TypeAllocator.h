#ifndef TypeAllocator_h
#define TypeAllocator_h

#include "Parser.h"
#include "SemanticAnalyzer.h"

static void allocate_type_in_constant(constant_t *cons);
static void allocate_type_in_variable_indicator(program_t *program, REF_SCOPE scope, variable_indicator_t *var_idr);
static void allocate_type_in_factor(program_t *program, REF_SCOPE scope, factor_t *factor);
static void allocate_type_in_term(program_t *program, REF_SCOPE scope, term_t *term);
static void allocate_type_in_simple_expression(program_t *program, REF_SCOPE scope, simple_expression_t *simp_expr);
static void allocate_type_in_expression(program_t *program, REF_SCOPE scope, expression_t *expr);
static void allocate_type_in_expressions(program_t *program, REF_SCOPE scope, expressions_t *exprs);
static void allocate_type_in_variable_indicators(program_t *program, REF_SCOPE scope, variable_indicators_t *var_idrs);
static void allocate_type_in_output_format(program_t *program, REF_SCOPE scope, output_format_t *fmt);
static void allocate_type_in_output_formats(program_t *program, REF_SCOPE scope, output_formats_t *fmts);
static void allocate_type_in_statement(program_t *program, REF_SCOPE scope, statement_t *stmt);
static void allocate_type_in_procedure(program_t *program, procedure_t *proc);
extern void allocate_type(program_t *program);

#endif
