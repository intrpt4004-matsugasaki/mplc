#ifndef Parser_h
#define Parser_h

#include "Scanner.h"

#define MAXELEMSIZE 100

typedef struct {
	char NAME[MAXSTRSIZE];

	int TYPE; // TINTEGER / TBOOLEAN / TCHAR / TARRAY
	int ARR_TYPE; // TINTEGER / TBOOLEAN / TCHAR / 0
	int ARR_NUM; // array[arr_num] of arr_type / 0
} var;

typedef struct {
	char NAME[MAXSTRSIZE];

	int VAR_LEN;
	var VAR[MAXELEMSIZE];
} proc;

typedef struct {
	char NAME[MAXSTRSIZE];

	int VAR_LEN;
	var VAR[MAXELEMSIZE];

	int PROC_LEN;
	proc PROC[MAXELEMSIZE];
} program;

extern program prog;

static void update_token();

enum {
	NORMAL	= 1,
	ERROR	= 0,
};

static int error(char *message);

static int is(const int TOKEN_CODE);
static int read(const int TOKEN_CODE);

extern int parse_program();

static int read_block();

static var vars[MAXELEMSIZE];
static int vars_len;
static int is_variable_declaration();
static int read_variable_declaration();

static int is_variable_names();
static int read_variable_names();

static char var_name[MAXELEMSIZE][MAXSTRSIZE];
static int var_name_len;
static int is_variable_name();
static int read_variable_name();

static int var_type;
static int read_type();

static int is_standard_type();
static int read_standard_type();

static int var_arr_num;
static int var_arr_type;
static int is_array_type();
static int read_array_type();

static int is_subprogram_declaration();
static int read_subprogram_declaration();

static int read_procedure_name();

static int is_formal_parameters();
static int read_formal_parameters();

static int is_compound_statement();
static int read_compound_statement();

static int read_statement();

static int is_condition_statement();
static int read_condition_statement();

static int is_iteration_statement();
static int read_iteration_statement();

static int is_exit_statement();
static int read_exit_statement();

static int is_call_statement();
static int read_call_statement();

static int read_expressions();

static int is_return_statement();
static int read_return_statement();

static int is_assignment_statement();
static int read_assignment_statement();

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

static int is_input_statement();
static int read_input_statement();

static int is_output_statement();
static int read_output_statement();

static int read_output_format();

static int read_empty_statement();

#endif
