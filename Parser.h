#ifndef Parser_h
#define Parser_h

#include "Scanner.h"

#define MAXPARAMSIZE	50
#define MAXVARSIZE		100
#define MAXPROCSIZE		100
#define MAXSTMTSIZE		1000

typedef struct {
	char NAME[MAXSTRSIZE];

	int TYPE; // TINTEGER / TBOOLEAN / TCHAR / TARRAY
	int ARR_TYPE; // TINTEGER / TBOOLEAN / TCHAR / 0
	int ARR_NUM; // array[arr_num] of arr_type / 0
} var;

typedef enum {
	SASSN, SCOND, SITER, SBRK, SCALL, SRET, SIN, SOUT, SCOMP, SEMPTY
} stmt_kind;

typedef struct _stmt {
	stmt_kind KIND;

	// assign statement
	char ASSN_TO[MAXSTRSIZE];

	// compound statement
	struct _stmt[MAXSTMTSIZE];
} stmt;

typedef struct {
	char NAME[MAXSTRSIZE];

	int PARAM_LEN;
	var PARAM[MAXPARAMSIZE];

	int VAR_LEN;
	var VAR[MAXVARSIZE];

	int STMT_LEN;
	stmt STMT[MAXSTMTSIZE];
} proc;

typedef struct {
	char NAME[MAXSTRSIZE];

	int VAR_LEN;
	var VAR[MAXVARSIZE];

	int PROC_LEN;
	proc PROC[MAXPROCSIZE];

	int STMT_LEN;
	stmt STMT[MAXSTMTSIZE];
} prog;

extern char *statementcode_to_str(const stmt_kind CODE);

static void update_token();

enum {
	NORMAL	= 1,
	ERROR	= 0,
};

static int error(char *message);

static int is(const int TOKEN_CODE);
static int read(const int TOKEN_CODE);

extern prog program;
extern int parse_program();

static int read_block();

static var var_tmp[MAXVARSIZE];
static int var_tmp_len;
static int is_variable_declaration();
static int read_variable_declaration();
static void store_variable_declaration();

static char varname_tmp[MAXVARSIZE][MAXSTRSIZE];
static int varname_tmp_len;
static int is_variable_names();
static int read_variable_names();

static int is_variable_name();
static int read_variable_name();

static int type_tmp;
static int read_type();

static int is_standard_type();
static int read_standard_type();

static int arr_num_tmp;
static int arr_type_tmp;
static int is_array_type();
static int read_array_type();

static proc proc_tmp;
static int is_subprogram_declaration();
static int read_subprogram_declaration();
static void store_subprogram_declaration();

static int read_procedure_name();

static int is_formal_parameters();
static int read_formal_parameters();

static stmt stmt_tmp[MAXSTMTSIZE];
static int stmt_tmp_len;
static int read_toplevel_statement();
static void store_toplevel_statement();

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
