#ifndef Debug_h
#define Debug_h

#include "Scanner.h"
#include "Parser.h"

/* Scanner enum/struct */
extern char *token_code_s(const token_code_t token_code);

/* Parser enum/struct */
extern char *standard_type_s(const standard_type_t std_type);
extern char *type_s(const type_t type);
extern char *multiplicative_operator_s(const multiplicative_operator_t mul_opr);
extern char *additive_operator_s(const additive_operator_t add_opr);
extern char *relational_operator_s(const relational_operator_t rel_opr);
extern char *boolean_s(const constant_t cons);

extern void print_constant(const constant_t cons);
extern void print_factor(const factor_t factor);
extern void print_term(const term_t term);
extern void print_simple_expression(const simple_expression_t simp_expr);
extern void print_expression(const expression_t expr);

#endif
