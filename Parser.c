#include "Parser.h"

/* --- token -------------------------------------------------------------------------- */
static token_t token;

static void update_token() {
	token.CODE = scan();
	strcpy(token.STR, string_attr);
	token.NUM = num_attr;
}

static int is(const int TOKEN_CODE) {
	return (token.CODE == TOKEN_CODE);
}

static void read(const int TOKEN_CODE, char *error_message) {
	if (!is(TOKEN_CODE))
		error(err_message);

	update_token();
}

static int error(char *message) {
	printf("[ERROR] Parser: %s (at line %d)\n", message, get_linenum());
	end_scan();

	exit(-1);
}
/* ------------------------------------------------------------------------------------ */

/* --- variable ----------------------------------------------------------------------- */
static int is_variable_name() {
	return is(TNAME);
}

static variable_t *read_variable_name() {
	variable_t *variable = malloc(sizeof(variable_t));
	variable->next = NULL;

	strcpy(variable->name, token.STR);
	read(TNAME, "variable name is not found.");

	return variable;
}

static int is_variable_names() {
	return is_variable_name();
}

static variable_t *read_variable_names() {
	variable_t *variable = read_variable_name();

	variable_t *last = variable;
	while (is(TCOMMA)) {
		read(TCOMMA, "',' is not found.");

		last->next = read_variable_name();
		last = last->next;
	}

	return variable;
}

static int is_standard_type() {
	return is(TINTEGER) || is(TBOOLEAN) || is(TCHAR);
}

static standard_type_t read_standard_type() {
	standard_type_t standard_type;

	if (is(TINTEGER)) {
		standard_type = INTEGER;
		read(TINTEGER, "{unreachable}");
		return standard_type;
	}

	if (is(TBOOLEAN)) {
		standard_type = BOOLEAN;
		read(TBOOLEAN, "{unreachable}");
		return standard_type;
	}

	if (is(TCHAR)) {
		standard_type = CHAR;
		read(TCHAR, "{unreachable}");
		return standard_type;
	}

	error("unmatch on standard_type.");
}

static int is_array_type() {
	return is(TARRAY);
}

static array_type_t read_array_type() {
	array_type_t array_type;

	read(TARRAY, "'array' is not found.");

	read(TLSQPAREN, "'[' is not found.");

	array_type.size = token.NUM;
	read(TNUMBER, "number is not found.");

	read(TRSQPAREN, "']' is not found.");

	read(TOF, "'of' is not found.");

	array_type.elem_type = read_standard_type();

	return array_type;
}

static type_t read_type() {
	type_t type;

	if (is_standard_type()) {
		type.kind = STD;
		type.standard = read_standard_type();
		return type;
	}

	if (is_array_type()) {
		type.kind = ARR;
		type.array = read_array_type();
		return type;
	}

	error("unmatched on read_type.");
}

static int is_variable_declaration() {
	return is(TVAR);
}

static variable_t *read_variable_declaration() {
	variable_t *variable;

	read(TVAR, "'var' is not found.");

	variable = read_variable_names();

	read(TCOLON, "':' is not found.");

	type_t type = read_type();
	variable_t *last = variable;
	for (; last->next != NULL; last = last->next) {
		last->type = type;
	}

	read(TSEMI, "';' is not found.");

	while (is_variable_names()) {
		last->next = read_variable_names();

		read(TCOLON, "':' is not found.");

		type = read_type();
		last = last->next;
		for (; last->next != NULL; last = last->next) {
			last->type = type;
		}

		read(TSEMI, "';' is not found.");
	}

	return variable;
}
/* ------------------------------------------------------------------------------------ */

/* --- procedure ---------------------------------------------------------------------- */
static procedure_t *read_procedure_name() {
	procedure_t *procedure = malloc(sizeof(procedure_t));

	strcpy(procedure->name, token.STR);
	read(TNAME, "procedure name is not found.");

	return procedure;
}

static int is_formal_parameters() {
	return is(TLPAREN);
}

static variable_t *read_formal_parameters() {
	variable_t *param;

	read(TLPAREN, "'(' is not found.");

	param = read_variable_names();

	read(TCOLON, "':' is not found.");

	type_t type = read_type();
	variable_t *last = param;
	for (; last->next != NULL; last = last->next) {
		last->type = type;
	}

	while (is(TSEMI)) {
		read(TSEMI, "';' is not found.");

		last->next = read_variable_names();

		read(TCOLON, "':' is not found.");

		type = read_type();
		last = last->next;
		for (; last->next != NULL; last = last->next) {
			last->type = type;
		}
	}

	if (!read(TRPAREN))
		return error("')' is not found.");

	return param;
}

static int is_subprogram_declaration() {
	return is(TPROCEDURE);
}

static procedure_t *read_subprogram_declaration() {
	procedure_t *procedure;

	read(TPROCEDURE, "'procedure' not found.");

	procedure = read_procedure_name();

	if (is_formal_parameters())
		procedure->param = read_formal_parameters();

	read(TSEMI, "';' is not found.");

	if (is_variable_declaration())
		procedure->var = read_variable_declaration();

	procedure->stmt = read_compound_statement();

	read(TSEMI, "';' is not found.");

	return procedure;
}
/* ------------------------------------------------------------------------------------ */

/* --- statement ---------------------------------------------------------------------- */
static int is_condition_statement() {
	return is(TIF);
}

static int read_condition_statement() {
	read(TIF, "'if' is not found.");

	read_expression();

	read(TTHEN, "'then' is not found.");

	read_statement();

	if (is(TELSE)) {
		read(TELSE, "'else' is not found.");

		read_statement();
	}

	return NORMAL;
}

static int is_iteration_statement() {
	return is(TWHILE);
}

static int read_iteration_statement() {
	read(TWHILE, "'while' is not found.");

	read_expression();

	read(TDO, "'do' is not found.");

	read_statement();

	return NORMAL;
}

static int is_exit_statement() {
	return is(TBREAK);
}

static int read_exit_statement() {
	read(TBREAK, "'break' is not found.");
	
	return NORMAL;
}

static int is_call_statement() {
	return is(TCALL);
}

static int read_call_statement() {
	read(TCALL, "'call' is not found.");

	read(TNAME, "callee procedure name not found.");

	if (is(TLPAREN)) {
		read(TLPAREN, "'(' is not found.");

		read_expressions();

		read(TRPAREN, "')' is not found.");
	}

	return NORMAL;
}

static int read_statement() {
	if (is_assignment_statement()) {
		read_assignment_statement();

	} else if (is_condition_statement()) {
		read_condition_statement();

	} else if (is_iteration_statement()) {
		if (!read_iteration_statement())

	} else if (is_exit_statement()) {
		if (!read_exit_statement())

	} else if (is_call_statement()) {
		if (!read_call_statement())

	} else if (is_return_statement()) {
		if (!read_return_statement())

	} else if (is_input_statement()) {
		if (!read_input_statement())

	} else if (is_output_statement()) {
		if (!read_output_statement())

	} else if (is_compound_statement()) {
		if (!read_compound_statement())

	} else if (!read_empty_statement()) {
	}

	return NORMAL;
}


static int is_compound_statement() {
	return is(TBEGIN);
}

static statement_t *read_compound_statement() {
	statement_t *statement;

	read(TBEGIN, "'begin' is not found.");

	statement = read_statement();

	statement_t *last = statement;
	while (is(TSEMI)) {
		read(TSEMI, "';' is not found.");

		for (; last->next != NULL; last = last->next);
		last->next = read_statement();
	}

	read(TEND, "'end' is not found.");

	return statement;
}
/* ------------------------------------------------------------------------------------ */

/* --- program ------------------------------------------------------------------------ */
static void read_block(program_t *program) {
	while (is_variable_declaration() || is_subprogram_declaration()) {
		if (is_variable_declaration()) {
			variable_t *last = program->var;
			if (last == NULL)
				last = read_variable_declaration();
			else {
				for (; last->next != NULL; last = last->next);
				last->next = read_variable_declaration();
			}
		}

		if (is_subprogram_declaration()) {
			procedure_t *last = program->proc;
			if (last == NULL)
				last = read_subprogram_declaration();
			else {
				for (; last->next != NULL; last = last->next);
				last->next = read_subprogram_declaration();
			}
		}
	}

	program->stmt = read_compound_statement();
}

static program_t read_program() {
	program_t program;
	program.var = NULL;
	program.stmt = NULL;
	program.proc = NULL;

	read(TPROGRAM, "'program' is not found.");

	strcpy(program.name, token.S);
	read(TNAME, "program name is not found.");

	read(TSEMI, "';' is not found.");

	read_block(program);
	read(TDOT, "'.' is not found.");

	return program;
}

extern program_t parse_program() {
	update_token();
	return read_program();
}
/* ------------------------------------------------------------------------------------ */













static int read_expressions() {
	read_expression();

	while (is(TCOMMA)) {
		read(TCOMMA, "',' is not found.");

		read_expression();
	}

	return NORMAL;
}

static int is_return_statement() {
	return is(TRETURN);
}

static int read_return_statement() {
	read(TRETURN, "'return' is not found.");

	return NORMAL;
}

static int is_assignment_statement() {
	return is_left_part();
}

static int read_assignment_statement() {
	read_left_part();

	read(TASSIGN, "':=' is not found.");

	read_expression();

	return NORMAL;
}

static int is_left_part() {
	return is_variable();
}

static int read_left_part() {
	read_variable();

	return NORMAL;
}

static int is_variable() {
	return is_variable_name();
}

static int read_variable() {
	read_variable_name();

	if (is(TLSQPAREN)) {
		read(TLSQPAREN, "'[' is not found.");

		read_expressions();

		read(TRSQPAREN, "']' is not found.");
	}

	return NORMAL;
}

static int is_expression() {
	return is_simple_expression();
}

static int read_expression() {
	read_simple_expression();

	while (is_relational_operator()) {
		read_relational_operator();

		read_simple_expression();
	}

	return NORMAL;
}

static int is_simple_expression() {
	return is(TPLUS) || is(TMINUS) || is_term();
}

static int read_simple_expression() {
	if (is(TPLUS))
		read(TPLUS, "'+' is not found.");		

	if (is(TMINUS))
		read(TMINUS, "'-' is not found.");		

	read_term();

	while (is_additive_operator()) {
		read_additive_operator();

		read_term();
	}

	return NORMAL;
}

static int is_term() {
	return is_factor();
}

static int read_term() {
	read_factor();

	while (is_multiplicative_operator()) {
		read_multiplicative_operator();

		read_factor();
	}

	return NORMAL;
}

static int is_factor() {
	return is_variable() || is_constant() ||
		is(TLPAREN) || is(TNOT) || is_standard_type();
}

static int read_factor() {
	if (is_variable()) {
		read_variable();
		return NORMAL;
	}

	if (is_constant()) {
		read_constant();
		return NORMAL;
	}

	if (is(TLPAREN)) {
		read(TLPAREN, "'(' is not found.");		
		read_expression();
		read(TRPAREN, "')' is not found.");
		return NORMAL;
	}

	if (is(TNOT)) {
		read(TNOT, "'not' is not found.");		
		read_factor();
		return NORMAL;
	}

	if (is_standard_type()) {
		read_standard_type();

		read(TLPAREN, "'(' is not found.");		

		read_expression();

		read(TRPAREN, "')' is not found.");
		return NORMAL;
	}

	error("unmachted on read_factor.");
}

static int is_constant() {
	return is(TNUMBER) || is(TFALSE) || is(TTRUE) || is(TSTRING);
}

static int read_constant() {
	if (is(TNUMBER)) {
		read(TNUMBER, "number is not found.");
		return NORMAL;
	}

	if (is(TFALSE)) {
		read(TFALSE, "'false' is not found.");
		return NORMAL;
	}

	if (is(TTRUE)) {
		read(TTRUE, "'true' is not found.");
		return NORMAL;
	}

	if (is(TSTRING)) {
		read(TSTRING, "string is not found.");
		return NORMAL;
	}

	error("unmachted on read_constant.");
}

static int is_multiplicative_operator() {
	return is(TSTAR) || is(TDIV) || is(TAND);
}

static int read_multiplicative_operator() {
	if (is(TSTAR)) {
		read(TSTAR, "'*' is not found.");
		return NORMAL;
	}

	if (is(TDIV)) {
		read(TDIV, "'div' is not found.");
		return NORMAL;
	}

	if (is(TAND)) {
		read(TAND, "'and' is not found.");
		return NORMAL;
	}

	error("unmatched on read_multiplicative_operator.");	
}

static int is_additive_operator() {
	return is(TPLUS) || is(TMINUS) || is(TOR);
}

static int read_additive_operator() {
	if (is(TPLUS)) {
		read(TPLUS, "'+' is not found.");
		return NORMAL;
	}

	if (is(TMINUS)) {
		read(TMINUS, "'-' is not found.");
		return NORMAL;
	}

	if (is(TOR)) {
		read(TOR, "'or' is not found.");
		return NORMAL;
	}

	error("unmatched on read_additive_operator.");	
}

static int is_relational_operator() {
	return is(TEQUAL) || is(TNOTEQ)
		|| is(TLE) || is(TLEEQ)
		|| is(TGR) || is(TGREQ);
}

static int read_relational_operator() {
	if (is(TEQUAL)) {
		read(TEQUAL, "'=' is not found.");
		return NORMAL;
	}

	if (is(TNOTEQ)) {
		read(TNOTEQ, "'<>' is not found.");
		return NORMAL;
	}

	if (is(TLE)) {
		read(TLE, "'<' is not found.");
		return NORMAL;
	}

	if (is(TLEEQ)) {
		read(TLEEQ, "'<=' is not found.");
		return NORMAL;
	}

	if (is(TGR)) {
		read(TGR, "'>' is not found.");
		return NORMAL;
	}

	if (is(TGREQ)) {
		read(TGREQ, "'>=' is not found.");
		return NORMAL;
	}

	error("unmatched on read_relational_operator.");
}

static int is_input_statement() {
	return is(TREAD) || is(TREADLN);
}

static int read_input_statement() {
	if (is(TREAD)) {
		read(TREAD, "'read' is not found.");

	} else if (is(TREADLN)) {
		read(TREADLN, "'readln' is not found.");

	} else
		error("unmatched on read_input_statement.");

	if (is(TLPAREN)) {
		read(TLPAREN, "'(' is not found.");

		read_variable();

		while (is(TCOMMA)) {
			read(TCOMMA, "',' is not found.");
			read_variable();
		}

		read(TRPAREN, "')' is not found.");
	}

	return NORMAL;
}

static int is_output_statement() {
	return is(TWRITE) || is(TWRITELN);
}

static int read_output_statement() {
	if (is(TWRITE)) {
		read(TWRITE, "'write' is not found.");

	} else if (is(TWRITELN)) {
		read(TWRITELN, "'writeln' is not found.");

	} else
		error("unmatched on read_output_statement.");

	if (is(TLPAREN)) {
		read(TLPAREN, "'(' is not found.");

		read_output_format();

		while (is(TCOMMA)) {
			read(TCOMMA, "',' is not found.");
			read_output_format();
		}

		read(TRPAREN, "')' is not found.");
	}

	return NORMAL;
}

static int read_output_format() {
	if (is_expression()) {
		read_expression();

		if (is(TCOLON)) {
			read(TCOLON, "':' is not found.");
			read(TNUMBER, "number is not found.");
		}

	} else if (is(TSTRING)) {
		read(TSTRING, "string is not found.");

	} else {
		error("unmatched on read_output_format.");
	}

	return NORMAL;
}

static void read_empty_statement() {
	// {empty}
}
