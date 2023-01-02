#ifndef Parser_h
#define Parser_h

#include "Scanner.h"

/* --- token -------------------------------------------------------------------------- */
typedef struct {
	int code;
	char string[MAXSTRSIZE];
	int number;
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
} variable_reference_t;

typedef struct {
	statement_t base;

	variable_reference_t target;
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

typedef struct {
	int a;
} simple_expression_t;

typedef struct {
	int a;
} term_t;

typedef struct {
	int a;
} output_format_t;




typedef enum {
	PLUS, MINUS, OR
} additive_operator_t;

typedef enum {
	ASTERISK, DIV, AND
} multiplicative_operator_t;

typedef enum {
	EQUAL, NOT_EQUAL,
	LESS, LESS_OR_EQUAL,
	GREATER, GREATER_OR_EQUAL
} relational_operator_t;

typedef struct {
	enum {
		NUMBER,
		TRUE, FALSE,
		STRING
	} kind;

	int number;
	char *string;
} constant_t;

typedef struct factor_t {
	enum {
		VAR_REF, CONST,
		EXPR, NOT_FACTOR,
		STD_TYPE
	} kind;

	variable_reference_t var_ref;
	constant_t cons;
	expression_t expr;
	struct factor_t *factor;
	standard_type_t std_type;
} factor_t;

// operator -------------------------------------------------
static int is_additive_operator();
static additive_operator_t read_additive_operator();

static int is_multiplicative_operator();
static multiplicative_operator_t read_multiplicative_operator();

static int is_relational_operator();
static relational_operator_t read_relational_operator();
// ----------------------------------------------------------

// expression -----------------------------------------------
static int is_constant();
static constant_t read_constant();

static int is_factor();
static factor_t read_factor();

static int is_term();
static term_t read_term();

static int is_simple_expression();
static simple_expression_t read_simple_expression();

static int is_expression();
static expression_t read_expression();
// ----------------------------------------------------------

// assignment statement -------------------------------------
static int is_variable();
static variable_reference_t read_variable();

static int is_left_part();
static variable_reference_t read_left_part();

static int is_assignment_statement();
static assignment_statement_t *read_assignment_statement();
// ----------------------------------------------------------

// condition statement --------------------------------------
static int is_condition_statement();
static condition_statement_t *read_condition_statement();
// ----------------------------------------------------------

// iteration statement --------------------------------------
static int is_iteration_statement();
static iteration_statement_t *read_iteration_statement();
// ----------------------------------------------------------

// exit statement -------------------------------------------
static int is_exit_statement();
static statement_t *read_exit_statement();
// ----------------------------------------------------------

// call statement -------------------------------------------
static expressions_t read_expressions();

static int is_call_statement();
static call_statement_t *read_call_statement();
// ----------------------------------------------------------

// return statement -----------------------------------------
static int is_return_statement();
static statement_t *read_return_statement();
// ----------------------------------------------------------

// input statement ------------------------------------------
static int is_input_statement();
static input_statement_t *read_input_statement();
// ----------------------------------------------------------

// output statement -----------------------------------------
static output_format_t read_output_format();

static int is_output_statement();
static output_statement_t *read_output_statement();
// ----------------------------------------------------------

// empty statement ------------------------------------------
static statement_t *read_empty_statement();
// ----------------------------------------------------------

// statement ------------------------------------------------
static statement_t *read_statement();
// ----------------------------------------------------------

// compound statement ---------------------------------------
static int is_compound_statement();
static statement_t *read_compound_statement();
// ----------------------------------------------------------
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
