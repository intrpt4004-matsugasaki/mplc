#ifndef Parser_h
#define Parser_h

#include "Scanner.h"

/* --- token -------------------------------------------------------------------------- */
typedef struct {
	int CODE;
	char STR[MAXSTRSIZE];
	int NUM;
} token_t;

static void update_token();

static int is(const int TOKEN_CODE);
static void read(const int TOKEN_CODE, char *error_message);
static void error(char *message);
/* ------------------------------------------------------------------------------------ */

/* --- variable ----------------------------------------------------------------------- */
typedef enum {
	INTEGER, BOOLEAN, CHAR
} standard_type_t;

typedef struct {
	standard_type_t elem_type;
	unsigned int size;
} array_type_t;

typedef struct {
	enum { STANDARD, ARRAY } kind;
	standard_type_t standard;
	array_type_t array;
} type_t;

typedef struct variable_t {
	char name[MAXSTRSIZE];
	type_t type;

	struct variable_t *next;
} variable_t;

static int is_variable_name();
static variable_t *read_variable_name();

static int is_variable_names();
static variable_t *read_variable_names();

static int is_standard_type();
static standard_type_t read_standard_type();

static int is_array_type();
static array_type_t read_array_type();

static type_t read_type();

static int is_variable_declaration();
static variable_t *read_variable_declaration();
/* ------------------------------------------------------------------------------------ */

/* --- statement ---------------------------------------------------------------------- */
typedef struct statement_t {
	enum {
		ASSIGN,
		CONDITION, ITERATION, EXIT,
		CALL, RETURN,
		INPUT, OUTPUT,
		EMPTY
	} kind;

	struct statement_t *next;
} statement_t;

typedef struct {
	int a; //
} expression_t;

typedef struct {
	char name[MAXSTRSIZE];

	int is_array;
	expression_t index;
} target_variable_t;

typedef struct {
	statement_t base;

	target_variable_t target;
	expression_t expr;
} assignment_statement_t;

typedef struct {
	statement_t base;

	expression_t branch_cond;
	statement_t *then_stmt;

	int has_else_stmt;
	statement_t *else_stmt;
} condition_statement_t;

typedef struct {
	statement_t base;

	expression_t loop_cond;
	statement_t *loop_stmt;
} iteration_statement_t;

typedef struct {
	 int a; //
} expressions_t;

typedef struct {
	statement_t base;

	char name[MAXSTRSIZE];
	expressions_t exprs;
} call_statement_t;

typedef struct {
	statement_t base;
} input_statement_t;

typedef struct {
	statement_t base;
} output_statement_t;

static int is_variable();
static target_variable_t read_variable();

static int is_left_part();
static target_variable_t read_left_part();



static expressions_t read_expressions();

static int is_expression();
static expression_t read_expression();

static int is_simple_expression();
static int read_simple_expression();

static int is_term();
static int read_term();

static int is_factor();
static int read_factor();

static int is_constant();
static int read_constant();

static int is_multiplicative_operator();
static int read_multiplicative_operator();

static int is_additive_operator();
static int read_additive_operator();

static int is_relational_operator();
static int read_relational_operator();

static int read_output_format();

enum {
	NORMAL = 0,
	ERROR
};



static int is_assignment_statement();
static assignment_statement_t *read_assignment_statement();

static int is_condition_statement();
static condition_statement_t *read_condition_statement();

static int is_iteration_statement();
static iteration_statement_t *read_iteration_statement();

static int is_exit_statement();
static statement_t *read_exit_statement();

static int is_call_statement();
static call_statement_t *read_call_statement();

static int is_return_statement();
static statement_t *read_return_statement();

static int is_input_statement();
static input_statement_t *read_input_statement();

static int is_output_statement();
static output_statement_t *read_output_statement();

static statement_t *read_empty_statement();

static statement_t *read_statement();

static int is_compound_statement();
static statement_t *read_compound_statement();
/* ------------------------------------------------------------------------------------ */

/* --- procedure ---------------------------------------------------------------------- */
typedef struct procedure_t {
	char name[MAXSTRSIZE];
	variable_t *param;
	variable_t *var;
	statement_t *stmt;

	struct procedure_t *next;
} procedure_t;

static procedure_t *read_procedure_name();

static int is_formal_parameters();
static variable_t *read_formal_parameters();

static int is_subprogram_declaration();
static procedure_t *read_subprogram_declaration();
/* ------------------------------------------------------------------------------------ */

/* --- program ------------------------------------------------------------------------ */
typedef struct {
	char name[MAXSTRSIZE];
	variable_t *var;
	statement_t *stmt;
	procedure_t *proc;
} program_t;

static void read_block(program_t *program);
static program_t read_program();

extern program_t parse_program();
/* ------------------------------------------------------------------------------------ */

#endif
