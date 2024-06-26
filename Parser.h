#ifndef Parser_h
#define Parser_h

#include "Scanner.h"

/* --- token -------------------------------------------------------------------------- */
typedef struct {
	int code;
	char string[MAXSTRSIZE];
	int number;

	/* debug information */
	int line_num;
} token_t;

static void update_token();

static int is(const token_code_t token_code);
static void read(const token_code_t token_code, char *error_message);
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

	/* debug information */
	int LINE_NUM;
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

	/* debug information */
	int LINE_NUM;
} statement_t;

// operator -------------------------------------------------
typedef enum {
	ASTERISK, DIV, AND
} multiplicative_operator_t;

typedef enum {
	PLUS, MINUS, OR
} additive_operator_t;

typedef enum {
	EQUAL, NOT_EQUAL,
	LESS, LESS_OR_EQUAL,
	GREATER, GREATER_OR_EQUAL
} relational_operator_t;

static int is_multiplicative_operator();
static multiplicative_operator_t read_multiplicative_operator();

static int is_additive_operator();
static additive_operator_t read_additive_operator();

static int is_relational_operator();
static relational_operator_t read_relational_operator();
// ----------------------------------------------------------

// expression -----------------------------------------------
typedef struct {
	enum {
		NUMBER,
		TRUE, FALSE,
		STRING
	} kind;

	int number;
	char string[MAXSTRSIZE];

	/* type allocation */
	type_t TYPE;

	/* debug information */
	int LINE_NUM;
} constant_t;

struct expression_t;

typedef struct {
	char name[MAXSTRSIZE];

	int is_array;
	struct expression_t *index;

	/* type allocation */
	type_t TYPE;

	/* debug information */
	int LINE_NUM;
} variable_indicator_t;

typedef struct factor_t {
	enum {
		VAR_IDR, CONST,
		EXPR, INVERT_FACTOR,
		CAST_EXPR
	} kind;

	variable_indicator_t var_idr;
	constant_t cons;
	struct expression_t *expr;
	struct factor_t *inv_factor;
	standard_type_t cast_std_type;
	struct expression_t *cast_expr;

	/* type allocation */
	type_t TYPE;

	/* debug information */
	int LINE_NUM;
} factor_t;

typedef struct term_t {
	factor_t factor;

	multiplicative_operator_t mul_opr;
	struct term_t *next;

	/* type allocation */
	type_t TYPE;

	/* debug information */
	int LINE_NUM;
} term_t;

typedef struct simple_expression_t {
	enum { NONE, POSITIVE, NEGATIVE } prefix;
	term_t term;

	additive_operator_t add_opr;
	struct simple_expression_t *next;

	/* type allocation */
	type_t TYPE;

	/* debug information */
	int LINE_NUM;
} simple_expression_t;

typedef struct expression_t {
	simple_expression_t simp_expr;

	relational_operator_t rel_opr;
	struct expression_t *next;

	/* type allocation */
	int is_start_point;
	type_t WHOLE_TYPE;
	type_t TYPE;

	/* debug information */
	int LINE_NUM;
} expression_t;

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
typedef struct {
	statement_t base;

	variable_indicator_t target_var_idr;
	expression_t expr;

	/* debug information */
	int LINE_NUM;
} assignment_statement_t;

static int is_variable();
static variable_indicator_t read_variable();

static int is_left_part();
static variable_indicator_t read_left_part();

static int is_assignment_statement();
static assignment_statement_t *read_assignment_statement();
// ----------------------------------------------------------

// condition statement --------------------------------------
typedef struct {
	statement_t base;

	expression_t cond;
	statement_t *then_stmt;

	int has_else_stmt;
	statement_t *else_stmt;

	/* debug information */
	int LINE_NUM;
} condition_statement_t;

static int is_condition_statement();
static condition_statement_t *read_condition_statement();
// ----------------------------------------------------------

// iteration statement --------------------------------------
typedef struct {
	statement_t base;

	expression_t cond;
	statement_t *loop_stmt;

	/* debug information */
	int LINE_NUM;
} iteration_statement_t;

static int is_iteration_statement();
static iteration_statement_t *read_iteration_statement();
// ----------------------------------------------------------

// exit statement -------------------------------------------
static int is_exit_statement();
static statement_t *read_exit_statement();
// ----------------------------------------------------------

// call statement -------------------------------------------
typedef struct expressions_t {
	expression_t expr;
	struct expressions_t *next;
} expressions_t;

typedef struct {
	statement_t base;

	char name[MAXSTRSIZE];
	expressions_t *param;

	/* debug information */
	int LINE_NUM;
} call_statement_t;

static expressions_t *read_expressions();

static int is_call_statement();
static call_statement_t *read_call_statement();
// ----------------------------------------------------------

// return statement -----------------------------------------
static int is_return_statement();
static statement_t *read_return_statement();
// ----------------------------------------------------------

// input statement ------------------------------------------
typedef struct variable_indicators_t {
	variable_indicator_t var_idr;
	struct variable_indicators_t *next;
} variable_indicators_t;

typedef struct {
	statement_t base;

	int lined;
	variable_indicators_t *target_var_idrs;

	/* debug information */
	int LINE_NUM;
} input_statement_t;

static int is_input_statement();
static input_statement_t *read_input_statement();
// ----------------------------------------------------------

// output statement -----------------------------------------
typedef struct {
	enum { EXPR_MODE, STR_MODE } kind;

	expression_t expr;
	int has_expr_num;
	int expr_num;

	char string[MAXSTRSIZE];

	/* type allocation */
	type_t TYPE;

	/* debug information */
	int LINE_NUM;
} output_format_t;

typedef struct output_formats_t {
	output_format_t format;
	struct output_formats_t *next;
} output_formats_t;

typedef struct {
	statement_t base;

	int lined;
	output_formats_t *formats;

	/* debug information */
	int LINE_NUM;
} output_statement_t;

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

	/* type allocation */
	int ARITY;
	type_t *TYPE;

	/* debug information */
	int LINE_NUM;
} procedure_t;

static void read_procedure_name(procedure_t *procedure);

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
