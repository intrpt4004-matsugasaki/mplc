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
static void read(const int TOKEN_CODE, char *err_message);
static void error(char *message);
/* ------------------------------------------------------------------------------------ */

/* --- variable ----------------------------------------------------------------------- */
enum standard_type_t {
	INTEGER, BOOLEAN, CHAR
};

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
static std_type_t read_standard_type();

static int is_array_type();
static arr_type_t read_array_type();

static type_t read_type();

static int is_variable_declaration();
static variable_t *read_variable_declaration();
/* ------------------------------------------------------------------------------------ */

/* --- procedure ---------------------------------------------------------------------- */
typedef struct {
	char name[MAXSTRSIZE];
	variable_t *param;
	variable_t *var;
	statement_t *stmt;
} procedure_t;

static procedure_t *read_procedure_name();

static int is_formal_parameters();
static variable_t *read_formal_parameters();

static int is_subprogram_declaration();
static procedure_t *read_subprogram_declaration();
/* ------------------------------------------------------------------------------------ */

/* --- statement ---------------------------------------------------------------------- */
typedef struct statement_t {
	enum { ASSIGN, CONDITION, ITERATION, EXIT, CALL, RETURN, INPUT, OUTPUT } kind;

	struct statement_t *next;
} statement_t;

static int is_assignment_statement();
static int read_assignment_statement();

static int is_condition_statement();
static int read_condition_statement();

static int is_iteration_statement();
static int read_iteration_statement();

static int is_exit_statement();
static int read_exit_statement();

static int is_call_statement();
static int read_call_statement();

static int is_return_statement();
static int read_return_statement();

static int is_input_statement();
static int read_input_statement();

static int is_output_statement();
static int read_output_statement();

static int read_empty_statement();

static int read_statement();

static int is_compound_statement();
static int read_compound_statement();
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




static int read_expressions();

static int is_left_part();
static int read_left_part();

static int is_variable();
static int read_variable();

static int is_expression();
static int read_expression();

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

#endif
