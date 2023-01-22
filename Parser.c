#include "Parser.h"

/* --- token -------------------------------------------------------------------------- */
static token_t token;

static void update_token() {
	token.code = scan();
	strcpy(token.string, string_attr);
	token.number = num_attr;

	/* debug information */
	token.line_num = get_linenum();
}

static int is(const int TOKEN_CODE) {
	return (token.code == TOKEN_CODE);
}

static void read(const int TOKEN_CODE, char *error_message) {
	if (!is(TOKEN_CODE))
		error(error_message);

	update_token();
}

static void error(char *message) {
	printf("[ERROR] Parser: %s (at line %d)\n", message, token.line_num);
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

	/* debug information */
	variable->LINE_NUM = token.line_num;

	strcpy(variable->name, token.string);
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
		read(TINTEGER, "'integer' is not found.");
		standard_type = INTEGER;
		return standard_type;
	}

	if (is(TBOOLEAN)) {
		read(TBOOLEAN, "'boolean' is not found.");
		standard_type = BOOLEAN;
		return standard_type;
	}

	if (is(TCHAR)) {
		read(TCHAR, "'char' is not found.");
		standard_type = CHAR;
		return standard_type;
	}

	error("unmatched on standard_type.");
}

static int is_array_type() {
	return is(TARRAY);
}

static array_type_t read_array_type() {
	array_type_t array_type;

	read(TARRAY, "'array' is not found.");

	read(TLSQPAREN, "'[' is not found.");

	array_type.size = token.number;
	read(TNUMBER, "number is not found.");

	read(TRSQPAREN, "']' is not found.");

	read(TOF, "'of' is not found.");

	array_type.elem_type = read_standard_type();

	return array_type;
}

static type_t read_type() {
	type_t type;

	if (is_standard_type()) {
		type.kind = STANDARD;
		type.standard = read_standard_type();
		return type;
	}

	if (is_array_type()) {
		type.kind = ARRAY;
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
	last->type = type;

	read(TSEMI, "';' is not found.");

	while (is_variable_names()) {
		last->next = read_variable_names();

		read(TCOLON, "':' is not found.");

		type = read_type();
		last = last->next;
		for (; last->next != NULL; last = last->next) {
			last->type = type;
		}
		last->type = type;

		read(TSEMI, "';' is not found.");
	}

	return variable;
}
/* ------------------------------------------------------------------------------------ */

/* --- statement ---------------------------------------------------------------------- */
// operator -------------------------------------------------
static int is_additive_operator() {
	return is(TPLUS) || is(TMINUS) || is(TOR);
}

static additive_operator_t read_additive_operator() {
	additive_operator_t add_opr;

	if (is(TPLUS)) {
		read(TPLUS, "'+' is not found.");
		add_opr = PLUS;
		return add_opr;
	}

	if (is(TMINUS)) {
		read(TMINUS, "'-' is not found.");
		add_opr = MINUS;
		return add_opr;
	}

	if (is(TOR)) {
		read(TOR, "'or' is not found.");
		add_opr = OR;
		return add_opr;
	}

	error("unmatched on read_additive_operator.");	
}

static int is_multiplicative_operator() {
	return is(TSTAR) || is(TDIV) || is(TAND);
}

static multiplicative_operator_t read_multiplicative_operator() {
	multiplicative_operator_t mul_opr;

	if (is(TSTAR)) {
		read(TSTAR, "'*' is not found.");
		mul_opr = ASTERISK;
		return mul_opr;
	}

	if (is(TDIV)) {
		read(TDIV, "'div' is not found.");
		mul_opr = DIV;
		return mul_opr;
	}

	if (is(TAND)) {
		read(TAND, "'and' is not found.");
		mul_opr = AND;
		return mul_opr;
	}

	error("unmatched on read_multiplicative_operator.");	
}

static int is_relational_operator() {
	return is(TEQUAL) || is(TNOTEQ)
		|| is(TLE) || is(TLEEQ)
		|| is(TGR) || is(TGREQ);
}

static relational_operator_t read_relational_operator() {
	relational_operator_t rel_opr;

	if (is(TEQUAL)) {
		read(TEQUAL, "'=' is not found.");
		rel_opr = EQUAL;
		return rel_opr;
	}

	if (is(TNOTEQ)) {
		read(TNOTEQ, "'<>' is not found.");
		rel_opr = NOT_EQUAL;
		return rel_opr;
	}

	if (is(TLE)) {
		read(TLE, "'<' is not found.");
		rel_opr = LESS;
		return rel_opr;
	}

	if (is(TLEEQ)) {
		read(TLEEQ, "'<=' is not found.");
		rel_opr = LESS_OR_EQUAL;
		return rel_opr;
	}

	if (is(TGR)) {
		read(TGR, "'>' is not found.");
		rel_opr = GREATER;
		return rel_opr;
	}

	if (is(TGREQ)) {
		read(TGREQ, "'>=' is not found.");
		rel_opr = GREATER_OR_EQUAL;
		return rel_opr;
	}

	error("unmatched on read_relational_operator.");
}
// ----------------------------------------------------------

// expression -----------------------------------------------
static int is_constant() {
	return is(TNUMBER) || is(TFALSE) || is(TTRUE) || is(TSTRING);
}

static constant_t read_constant() {
	constant_t constant;

	/* debug information */
	constant.LINE_NUM = token.line_num;


	if (is(TNUMBER)) {
		read(TNUMBER, "number is not found.");
		constant.kind = NUMBER;
		constant.number = token.number;
		return constant;
	}

	if (is(TFALSE)) {
		read(TFALSE, "'false' is not found.");
		constant.kind = FALSE;
		return constant;
	}

	if (is(TTRUE)) {
		read(TTRUE, "'true' is not found.");
		constant.kind = TRUE;
		return constant;
	}

	if (is(TSTRING)) {
		read(TSTRING, "string is not found.");
		constant.kind = STRING;
		strcpy(constant.string, token.string);
		return constant;
	}

	error("unmachted on read_constant.");
}

static int is_factor() {
	return is_variable() || is_constant() ||
		is(TLPAREN) || is(TNOT) || is_standard_type();
}

static factor_t read_factor() {
	factor_t factor;

	/* debug information */
	factor.LINE_NUM = token.line_num;

	if (is_variable()) {
		factor.kind = VAR_IDR;
		factor.var_idr = read_variable();
	}

	else if (is_constant()) {
		factor.kind = CONST;
		factor.cons = read_constant();
	}

	else if (is(TLPAREN)) {
		read(TLPAREN, "'(' is not found.");		

		factor.kind = EXPR;
		factor.expr = malloc(sizeof(expression_t));
		*factor.expr = read_expression();

		read(TRPAREN, "')' is not found.");
	}

	else if (is(TNOT)) {
		read(TNOT, "'not' is not found.");		

		factor.kind = INVERT_FACTOR;
		factor.inv_factor = malloc(sizeof(factor_t));
		*factor.inv_factor = read_factor();
	}

	else if (is_standard_type()) {
		factor.kind = CAST_EXPR;

		factor.cast_std_type = read_standard_type();

		read(TLPAREN, "'(' is not found.");		

		factor.cast_expr = malloc(sizeof(expression_t));
		*factor.cast_expr = read_expression();

		read(TRPAREN, "')' is not found.");
	}
	
	else {
		error("unmachted on read_factor.");
	}

	return factor;
}

static int is_term() {
	return is_factor();
}

static term_t read_term() {
	term_t term;
	term.next = NULL;

	/* debug information */
	term.LINE_NUM = token.line_num;

	term.factor = read_factor();

	term_t *last = &term;
	while (is_multiplicative_operator()) {
		last->mul_opr = read_multiplicative_operator();

		last->next = malloc(sizeof(term_t));
		last = last->next;
		last->next = NULL;

		last->factor = read_factor();
	}

	return term;
}

static int is_simple_expression() {
	return is(TPLUS) || is(TMINUS) || is_term();
}

static simple_expression_t read_simple_expression() {
	simple_expression_t simp_expr;
	simp_expr.next = NULL;
	simp_expr.prefix = NONE;

	/* debug information */
	simp_expr.LINE_NUM = token.line_num;

	if (is(TPLUS)) {
		read(TPLUS, "'+' is not found.");
		simp_expr.prefix = POSITIVE;
	} else if (is(TMINUS)) {
		read(TMINUS, "'-' is not found.");		
		simp_expr.prefix = NEGATIVE;
	}

	simp_expr.term = read_term();

	simple_expression_t *last = &simp_expr;
	while (is_additive_operator()) {
		last->add_opr = read_additive_operator();

		last->next = malloc(sizeof(simple_expression_t));
		last = last->next;
		last->next = NULL;

		last->term = read_term();
	}

	return simp_expr;
}

static int is_expression() {
	return is_simple_expression();
}

static expression_t read_expression() {
	expression_t expr;
	expr.next = NULL;

	/* debug information */
	expr.LINE_NUM = token.line_num;

	expr.simp_expr = read_simple_expression();

	expression_t *last = &expr;
	while (is_relational_operator()) {
		last->rel_opr = read_relational_operator();

		last->next = malloc(sizeof(expression_t));
		last = last->next;
		last->next = NULL;

		last->simp_expr = read_simple_expression();
	}

	return expr;
}
// ----------------------------------------------------------

// assignment statement -------------------------------------
static int is_variable() {
	return is_variable_name();
}

static variable_indicator_t read_variable() {
	variable_indicator_t target_var_idr;
	target_var_idr.is_array = 0;
	target_var_idr.index = NULL;

	/* debug information */
	target_var_idr.LINE_NUM = token.line_num;

	variable_t *variable = read_variable_name();
	strcpy(target_var_idr.name, variable->name);

	if (is(TLSQPAREN)) {
		read(TLSQPAREN, "'[' is not found.");

		target_var_idr.is_array = 1;
		target_var_idr.index = malloc(sizeof(expression_t));
		*target_var_idr.index = read_expression();

		read(TRSQPAREN, "']' is not found.");
	}

	return target_var_idr;
}

static int is_left_part() {
	return is_variable();
}

static variable_indicator_t read_left_part() {
	return read_variable();
}

static int is_assignment_statement() {
	return is_left_part();
}

static assignment_statement_t *read_assignment_statement() {
	assignment_statement_t *assn_stmt = malloc(sizeof(assignment_statement_t));
	assn_stmt->base.kind = ASSIGN;
	assn_stmt->base.next = NULL;

	/* debug information */
	assn_stmt->LINE_NUM = token.line_num;

	assn_stmt->target_var_idr = read_left_part();

	read(TASSIGN, "':=' is not found.");

	assn_stmt->expr = read_expression();

	return assn_stmt;
}
// ----------------------------------------------------------

// condition statement --------------------------------------
static int is_condition_statement() {
	return is(TIF);
}

static condition_statement_t *read_condition_statement() {
	condition_statement_t *cond_stmt = malloc(sizeof(condition_statement_t));
	cond_stmt->base.kind = CONDITION;
	cond_stmt->base.next = NULL;
	cond_stmt->has_else_stmt = 0;

	/* debug information */
	cond_stmt->LINE_NUM = token.line_num;

	read(TIF, "'if' is not found.");

	cond_stmt->cond = read_expression();

	read(TTHEN, "'then' is not found.");

	cond_stmt->then_stmt = read_statement();

	if (is(TELSE)) {
		read(TELSE, "'else' is not found.");

		cond_stmt->has_else_stmt = 1;
		cond_stmt->else_stmt = read_statement();
	}

	return cond_stmt;
}
// ----------------------------------------------------------

// iteration statement --------------------------------------
static int is_iteration_statement() {
	return is(TWHILE);
}

static iteration_statement_t *read_iteration_statement() {
	iteration_statement_t *iter_stmt = malloc(sizeof(iteration_statement_t));
	iter_stmt->base.kind = ITERATION;
	iter_stmt->base.next = NULL;

	/* debug information */
	iter_stmt->LINE_NUM = token.line_num;

	read(TWHILE, "'while' is not found.");

	iter_stmt->cond = read_expression();

	read(TDO, "'do' is not found.");

	iter_stmt->loop_stmt = read_statement();

	return iter_stmt;
}
// ----------------------------------------------------------

// exit statement -------------------------------------------
static int is_exit_statement() {
	return is(TBREAK);
}

static statement_t *read_exit_statement() {
	statement_t *stmt = malloc(sizeof(statement_t));
	stmt->kind = EXIT;
	stmt->next = NULL;

	/* debug information */
	stmt->LINE_NUM = token.line_num;

	read(TBREAK, "'break' is not found.");

	return stmt;
}
// ----------------------------------------------------------

// call statement -------------------------------------------
static expressions_t *read_expressions() {
	expressions_t *exprs = malloc(sizeof(expressions_t));
	exprs->next = NULL;

	exprs->expr = read_expression();

	expressions_t *last = exprs;
	while (is(TCOMMA)) {
		read(TCOMMA, "',' is not found.");

		last->next = malloc(sizeof(expressions_t));
		last->next->next = NULL;

		last = last->next;

		last->expr = read_expression();
	}

	return exprs;
}

static int is_call_statement() {
	return is(TCALL);
}

static call_statement_t *read_call_statement() {
	call_statement_t *call_stmt = malloc(sizeof(call_statement_t));
	call_stmt->base.kind = CALL;
	call_stmt->base.next = NULL;

	/* debug information */
	call_stmt->LINE_NUM = token.line_num;

	read(TCALL, "'call' is not found.");
	
	/* debug information */
	call_stmt->LINE_NUM = token.line_num;

	strcpy(call_stmt->name, token.string);
	read(TNAME, "callee procedure name not found.");

	if (is(TLPAREN)) {
		read(TLPAREN, "'(' is not found.");

		call_stmt->param = read_expressions();

		read(TRPAREN, "')' is not found.");
	}

	return call_stmt;
}
// ----------------------------------------------------------

// return statement -----------------------------------------
static int is_return_statement() {
	return is(TRETURN);
}

static statement_t *read_return_statement() {
	statement_t *stmt = malloc(sizeof(statement_t));
	stmt->kind = RETURN;
	stmt->next = NULL;

	/* debug information */
	stmt->LINE_NUM = token.line_num;

	read(TRETURN, "'return' is not found.");

	return stmt;
}
// ----------------------------------------------------------

// input statement ------------------------------------------
static int is_input_statement() {
	return is(TREAD) || is(TREADLN);
}

static input_statement_t *read_input_statement() {
	input_statement_t *input_stmt = malloc(sizeof(input_statement_t));
	input_stmt->base.kind = INPUT;
	input_stmt->base.next = NULL;

	/* debug information */
	input_stmt->LINE_NUM = token.line_num;

	if (is(TREAD)) {
		read(TREAD, "'read' is not found.");
		input_stmt->lined = 0;

	} else if (is(TREADLN)) {
		read(TREADLN, "'readln' is not found.");
		input_stmt->lined = 1;

	} else
		error("unmatched on read_input_statement.");

	if (is(TLPAREN)) {
		read(TLPAREN, "'(' is not found.");

		input_stmt->target_var_idrs = malloc(sizeof(variable_indicators_t));
		input_stmt->target_var_idrs->next = NULL;
		input_stmt->target_var_idrs->var_idr = read_variable();

		variable_indicators_t *last = input_stmt->target_var_idrs;
		while (is(TCOMMA)) {
			read(TCOMMA, "',' is not found.");

			last->next = malloc(sizeof(variable_indicators_t));
			last = last->next;
			last->next = NULL;

			last->var_idr = read_variable();
		}

		read(TRPAREN, "')' is not found.");
	}

	return input_stmt;
}
// ----------------------------------------------------------

// output statement -----------------------------------------
static output_format_t read_output_format() {
	output_format_t format;
	format.has_expr_num = 0;

	/* debug information */
	format.LINE_NUM = token.line_num;

	if (is(TSTRING)) {
		format.kind = STR_MODE;

		strcpy(format.string, token.string);
		read(TSTRING, "string is not found.");
	}
	else if (is_expression()) {
		format.kind = EXPR_MODE;
		format.expr = read_expression();

		if (is(TCOLON)) {
			read(TCOLON, "':' is not found.");

			format.has_expr_num = 1;
			format.expr_num = token.number;
			read(TNUMBER, "number is not found.");
		}
	}
	else {
		error("unmatched on read_output_format.");
	}

	return format;
}

static int is_output_statement() {
	return is(TWRITE) || is(TWRITELN);
}

static output_statement_t *read_output_statement() {
	output_statement_t *output_stmt = malloc(sizeof(output_statement_t));
	output_stmt->base.kind = OUTPUT;
	output_stmt->base.next = NULL;
	output_stmt->formats = NULL;

	/* debug information */
	output_stmt->LINE_NUM = token.line_num;

	if (is(TWRITE)) {
		read(TWRITE, "'write' is not found.");
		output_stmt->lined = 0;

	} else if (is(TWRITELN)) {
		read(TWRITELN, "'writeln' is not found.");
		output_stmt->lined = 1;

	} else
		error("unmatched on read_output_statement.");

	if (is(TLPAREN)) {
		read(TLPAREN, "'(' is not found.");

		output_stmt->formats = malloc(sizeof(output_formats_t));
		output_stmt->formats->next = NULL;
		output_stmt->formats->format = read_output_format();

		output_formats_t *last = output_stmt->formats;
		while (is(TCOMMA)) {
			read(TCOMMA, "',' is not found.");

			last->next = malloc(sizeof(output_formats_t));
			last = last->next;
			last->next = NULL;

			last->format = read_output_format();
		}

		read(TRPAREN, "')' is not found.");
	}

	return output_stmt;
}
// ----------------------------------------------------------

// empty statement ------------------------------------------
static statement_t *read_empty_statement() {
	statement_t *stmt = malloc(sizeof(statement_t));
	stmt->kind = EMPTY;
	stmt->next = NULL;

	/* debug information */
	stmt->LINE_NUM = token.line_num;

	return stmt;
}
// ----------------------------------------------------------

// statement ------------------------------------------------
static statement_t *read_statement() {
	if (is_assignment_statement())
		return (statement_t *)(read_assignment_statement());

	if (is_condition_statement())
		return (statement_t *)(read_condition_statement());

	if (is_iteration_statement())
		return (statement_t *)(read_iteration_statement());

	if (is_exit_statement())
		return read_exit_statement();

	if (is_call_statement())
		return (statement_t *)(read_call_statement());

	if (is_return_statement())
		return read_return_statement();

	if (is_input_statement())
		return (statement_t *)(read_input_statement());

	if (is_output_statement())
		return (statement_t *)(read_output_statement());

	if (is_compound_statement())
		return read_compound_statement();

	return read_empty_statement();
}// ----------------------------------------------------------

// compound statement ---------------------------------------
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
// ----------------------------------------------------------
/* ------------------------------------------------------------------------------------ */

/* --- procedure ---------------------------------------------------------------------- */
static void read_procedure_name(procedure_t *procedure) {
	strcpy(procedure->name, token.string);
	read(TNAME, "procedure name is not found.");
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
	last->type = type;

	while (is(TSEMI)) {
		read(TSEMI, "';' is not found.");

		last->next = read_variable_names();

		read(TCOLON, "':' is not found.");

		type = read_type();
		last = last->next;
		for (; last->next != NULL; last = last->next) {
			last->type = type;
		}
		last->type = type;
	}

	read(TRPAREN, "')' is not found.");

	return param;
}

static int is_subprogram_declaration() {
	return is(TPROCEDURE);
}

static procedure_t *read_subprogram_declaration() {
	procedure_t *procedure = malloc(sizeof(procedure_t));
	procedure->next = NULL;
	procedure->param = NULL;
	procedure->var = NULL;
	procedure->stmt = NULL;

	/* debug information */
	procedure->LINE_NUM = token.line_num;

	read(TPROCEDURE, "'procedure' not found.");

	read_procedure_name(procedure);

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

/* --- program ------------------------------------------------------------------------ */
static void read_block(program_t *program) {
	while (is_variable_declaration() || is_subprogram_declaration()) {
		if (is_variable_declaration()) {
			if (program->var == NULL) {
				program->var = read_variable_declaration();
			} else {
				variable_t *last = program->var;
				for (; last->next != NULL; last = last->next);
				last->next = read_variable_declaration();
			}

		} else if (is_subprogram_declaration()) {
			if (program->proc == NULL) {
				program->proc = read_subprogram_declaration();
			} else {
				procedure_t *last = program->proc;
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

	strcpy(program.name, token.string);
	read(TNAME, "program name is not found.");

	read(TSEMI, "';' is not found.");

	read_block(&program);
	read(TDOT, "'.' is not found.");

	return program;
}

extern program_t parse_program() {
	update_token();

	program_t program = read_program();
	end_scan();

	return program;
}
/* ------------------------------------------------------------------------------------ */
