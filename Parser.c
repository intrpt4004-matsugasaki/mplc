#include "Parser.h"

static int token;
static void update_token() {
	token = scan();
}

static int error(char *message) {
	printf("[ERROR] Parser: %s (at line %d)\n", message, get_linenum());
	end_scan();

	return ERROR;
}

static int is(const int TOKEN_CODE) {
	return (token == TOKEN_CODE)?
		NORMAL : ERROR; // {redundant}
}

static int read(const int TOKEN_CODE) {
	const int match = is(TOKEN_CODE);
	update_token();

	return (match)? NORMAL : ERROR; // {redundant}
}

prog program;
extern int parse_program() {
	// init program
	program.VAR_LEN = 0;

	update_token();

	if (!read(TPROGRAM)) {
		return error("'program' is not found.");
	}

	// string_attr -> program.NAME
	strcpy(program.NAME, string_attr);

	if (!read(TNAME)) {
		return error("program name is not found.");
	}

	if (!read(TSEMI)) {
		return error("';' is not found.");
	}

	if (!read_block()) {
		return error("read_block failed.");
	}

	if (!read(TDOT)) {
		return error("'.' is not found.");
	}

	return NORMAL;
}

static int read_block() {
	while (is_variable_declaration() || is_subprogram_declaration()) {
		if (is_variable_declaration()) {
			if (!read_variable_declaration())
				return error("read_variable_declaration failed.");
			store_variable_declaration();
		}

		if (is_subprogram_declaration()) {
			if (!read_subprogram_declaration())
				return error("read_subprogram_declaration failed.");
			store_subprogram_declaration();
		}
	}

	if (!read_compound_statement())
		return error("read_compound_statement failed.");

	return NORMAL;
}

static int is_variable_declaration() {
	return is(TVAR);
}

static var var_tmp[MAXELEMSIZE];
static int var_tmp_len;
static int read_variable_declaration() {
	// init var_tmp
	var_tmp_len = 0;
	memset(var_tmp, 0, sizeof(var_tmp));

	if (!read(TVAR))
		return error("'var' is not found.");

	if (!read_variable_names())
		return error("read_variable_names failed.");

	if (!read(TCOLON))
		return error("':' is not found.");

	if (!read_type())
		return error("read_type failed.");

	if (!read(TSEMI))
		return error("';' is not found.");

	// varname_tmp -> var_tmp[]
	for (int i = 0; i < varname_tmp_len; i++) {
		strcpy(var_tmp[i].NAME	, varname_tmp[i]);

		var_tmp[i].TYPE			= type_tmp;

		if (type_tmp == TARRAY) {
			var_tmp[i].ARR_TYPE	= arr_type_tmp;
			var_tmp[i].ARR_NUM	= arr_num_tmp;

		} else {
			var_tmp[i].ARR_TYPE	= 0;
			var_tmp[i].ARR_NUM	= 0;
		}
	}
	var_tmp_len = varname_tmp_len;

	while (is_variable_names()) {
		if (!read_variable_names())
			return error("read_variable_names failed.");

		if (!read(TCOLON))
			return error("':' is not found.");

		if (!read_type())
			return error("read_type failed.");

		if (!read(TSEMI))
			return error("';' is not found.");

		// varname_tmp -> var_tmp[]
		for (int i = 0; i < varname_tmp_len; i++) {
			strcpy(var_tmp[var_tmp_len + i].NAME	, varname_tmp[i]);

			var_tmp[var_tmp_len + i].TYPE			= type_tmp;

			if (type_tmp == TARRAY) {
				var_tmp[var_tmp_len + i].ARR_TYPE	= arr_type_tmp;
				var_tmp[var_tmp_len + i].ARR_NUM	= arr_num_tmp;

			} else {
				var_tmp[var_tmp_len + i].ARR_TYPE	= 0;
				var_tmp[var_tmp_len + i].ARR_NUM	= 0;
			}
		}
		var_tmp_len += varname_tmp_len;
	}

	return NORMAL;
}

static void store_variable_declaration() {
	// var_tmp -> program.VAR+
	for (int i = 0; i < var_tmp_len; i++) {
		strcpy(program.VAR[program.VAR_LEN + i].NAME	, var_tmp[i].NAME);

		program.VAR[program.VAR_LEN + i].TYPE			= var_tmp[i].TYPE;
		program.VAR[program.VAR_LEN + i].ARR_TYPE		= var_tmp[i].ARR_TYPE;
		program.VAR[program.VAR_LEN + i].ARR_NUM		= var_tmp[i].ARR_NUM;
	}

	prog.VAR_LEN += var_tmp_len;
}

static int is_variable_names() {
	return is_variable_name();
}

static char varname_tmp[MAXELEMSIZE][MAXSTRSIZE];
static int varname_tmp_len;
static int read_variable_names() {
	// init var_name_tmp
	varname_tmp_len = 0;
	memset(varname_tmp, 0, sizeof(varname_tmp));

	if (!read_variable_name())
		return error("read_variable_name failed.");

	while (is(TCOMMA)) {
		if (!read(TCOMMA))
			return error("',' is not found.");

		if (!read_variable_name())
			return error("read_variable_name failed.");
	}

	return NORMAL;
}

static int is_variable_name() {
	return is(TNAME);
}

static int read_variable_name() {
	// 	string_attr -> varname_tmp[]
	strcpy(varname_tmp[varname_tmp_len++], string_attr);

	if (!read(TNAME))
		return error("variable name is not found.");
}

static int type_tmp;
static int read_type() {
	// init type_tmp
	type_tmp = 0;

	if (is_standard_type()) {
		if (!read_standard_type())
			return error("read_standard_type failed.");
		return NORMAL;
	}

	if (is_array_type()) {
		if (!read_array_type())
			return error("read_array_type failed.");
		return NORMAL;
	}

	return error("unmatched on read_type.");
}

static int is_standard_type() {
	return is(TINTEGER) || is(TBOOLEAN) || is(TCHAR);
}

static int read_standard_type() {
	// token -> type_tmp
	type_tmp = token;

	if (is(TINTEGER)) {
		if (!read(TINTEGER))
			return error("{unreachable}");
		return NORMAL;
	}

	if (is(TBOOLEAN)) {
		if (!read(TBOOLEAN))
			return error("{unreachable}");
		return NORMAL;
	}

	if (is(TCHAR)) {
		if (!read(TCHAR))
			return error("{unreachable}");
		return NORMAL;
	}

	return error("unmatch on standard_type.");
}

static int is_array_type() {
	return is(TARRAY);
}

static int arr_num_tmp;
static int arr_type_tmp;
static int read_array_type() {
	// init arr_num_tmp, arr_type_tmp
	arr_num_tmp = 0;
	arr_type_tmp = 0;

	// token -> type_tmp
	type_tmp = token;

	if (!read(TARRAY))
		return error("'array' is not found.");

	if (!read(TLSQPAREN))
		return error("'[' is not found.");

	// num_attr -> arr_num_tmp
	arr_num_tmp = num_attr;

	if (!read(TNUMBER))
		return error("number is not found.");

	if (!read(TRSQPAREN))
		return error("']' is not found.");

	if (!read(TOF))
		return error("'of' is not found.");

	// token -> arr_type_tmp
	arr_type_tmp = token;

	if (!read_standard_type())
		return error("read_standard_type failed.");

	return NORMAL;
}

static int is_subprogram_declaration() {
	return is(TPROCEDURE);
}

static proc proc_tmp;
static int read_subprogram_declaration() {
	// init proc_tmp
	proc_tmp.PARAM_LEN = 0;
	memset(proc_tmp.PARAM, 0, sizeof(proc_tmp.PARAM));
	proc_tmp.VAR_LEN = 0;
	memset(proc_tmp.VAR, 0, sizeof(proc_tmp.VAR));

	if (!read(TPROCEDURE))
		return error("'procedure' not found.");

	if (!read_procedure_name())
		return error("read_procedure_name not found.");

	if (is_formal_parameters())
		if (!read_formal_parameters())
			return error("read_formal_parameters failed.");

	if (!read(TSEMI))
		return error("';' is not found.");

	if (is_variable_declaration())
		if (!read_variable_declaration())
			return error("read_variable_declaration failed.");

	// var_tmp -> proc_tmp.VAR
	for (int i = 0; i < var_tmp_len; i++) {
		strcpy(program.VAR[program.VAR_LEN + i].NAME	, var_tmp[i].NAME);

		prog.VAR[prog.VAR_LEN + i].TYPE					= var_tmp[i].TYPE;
		prog.VAR[prog.VAR_LEN + i].ARR_TYPE				= var_tmp[i].ARR_TYPE;
		prog.VAR[prog.VAR_LEN + i].ARR_NUM				= var_tmp[i].ARR_NUM;
	}

	prog.VAR_LEN += var_tmp_len;

	}
	proc_tmp.VAR_LEN = var_tmp_len;

	if (!read_compound_statement())
		return error("read_compound_statement failed.");

	if (!read(TSEMI))
		return error("';' is not found.");

	return NORMAL;
}

static void store_subprogram_declaration() {
	// proc_tmp -> program.PROC[]
	const int this = program.PROC_LEN;

	// proc_tmp.NAME -> program.PROC[].NAME
	strcpy(program.PROC[this].NAME, proc_tmp.NAME);

	// proc_tmp.PARAM -> program.PROC[].PARAM
	program.PROC[this].PARAM_LEN = proc_tmp.PARAM_LEN;
	for (int i = 0; i < proc_tmp.PARAM_LEN; i++) {
		strcpy(program.PROC[this].PARAM[i].NAME	, proc_tmp.PARAM[i].NAME);

		program.PROC[this].PARAM[i].TYPE		= proc_tmp.PARAM[i].TYPE;
		program.PROC[this].PARAM[i].ARR_TYPE	= proc_tmp.PARAM[i].ARR_TYPE;
		program.PROC[this].PARAM[i].ARR_NUM		= proc_tmp.PARAM[i].ARR_NUM;
	}

	// proc_tmp.VAR -> program.PROC[].VAR
	program.PROC[this].VAR_LEN = proc_tmp.VAR_LEN;
	for (int i = 0; i < proc_tmp.VAR_LEN; i++) {
		strcpy(program.PROC[this].VAR[i].NAME	, proc_tmp.VAR[i].NAME);

		program.PROC[this].VAR[i].TYPE			= proc_tmp.VAR[i].TYPE;
		program.PROC[this].VAR[i].ARR_TYPE		= proc_tmp.VAR[i].ARR_TYPE;
		program.PROC[this].VAR[i].ARR_NUM		= proc_tmp.VAR[i].ARR_NUM;
	}

	program.PROC_LEN++;
}

static int read_procedure_name() {
	// string_attr -> proc_tmp.NAME
	strcpy(proc_tmp.NAME, string_attr);

	if (!read(TNAME))
		return error("procedure name is not found.");
	
	return NORMAL;
}

static int is_formal_parameters() {
	return is(TLPAREN);
}

static int read_formal_parameters() {
	if (!read(TLPAREN))
		return error("'(' is not found.");

	if (!read_variable_names())
		return error("read_variable_names failed.");

	if (!read(TCOLON))
		return error("':' is not found.");

	if (!read_type())
		return error("read_type failed.");

	// varname_tmp -> proc_tmp.PARAM
	for (int i = 0; i < varname_tmp_len; i++) {
		strcpy(proc_tmp.PARAM[i].NAME	, varname_tmp[i]);

		proc_tmp.PARAM[i].TYPE			= type_tmp;

		if (type_tmp == TARRAY) {
			proc_tmp.PARAM[i].ARR_TYPE	= arr_type_tmp;
			proc_tmp.PARAM[i].ARR_NUM	= arr_num_tmp;

		} else {
			proc_tmp.PARAM[i].ARR_TYPE	= 0;
			proc_tmp.PARAM[i].ARR_NUM	= 0;
		}
	}
	proc_tmp.PARAM_LEN = varname_tmp_len;

	while (is(TSEMI)) {
		if (!read(TSEMI))
			return error("';' is not found.");

		if (!read_variable_names())
			return error("read_variable_names failed.");

		// varname_tmp -> proc_tmp.PARAM
		for (int i = 0; i < varname_tmp_len; i++) {
			strcpy(proc_tmp.PARAM[proc_tmp.PARAM_LEN + i].NAME		, varname_tmp[i]);

			proc_tmp.PARAM[proc_tmp.PARAM_LEN + i].TYPE				= type_tmp;

			if (type_tmp == TARRAY) {
				proc_tmp.PARAM[proc_tmp.PARAM_LEN + i].ARR_TYPE		= arr_type_tmp;
				proc_tmp.PARAM[proc_tmp.PARAM_LEN + i].ARR_NUM		= arr_num_tmp;

			} else {
				proc_tmp.PARAM[proc_tmp.PARAM_LEN + i].ARR_TYPE		= 0;
				proc_tmp.PARAM[proc_tmp.PARAM_LEN + i].ARR_NUM		= 0;
			}
		}
		proc_tmp.PARAM_LEN += varname_tmp_len;

		if (!read(TCOLON))
			return error("':' is not found.");

		if (!read_type())
			return error("read_type failed.");
	}

	if (!read(TRPAREN))
		return error("')' is not found.");

	return NORMAL;
}

static int is_compound_statement() {
	return is(TBEGIN);
}

static int read_compound_statement() {
	if (!read(TBEGIN))
		return error("'begin' is not found.");

	if (!read_statement())
		return error("read_statement failed.");

	while (is(TSEMI)) {
		if (!read(TSEMI))
			return error("';' is not found.");

		if (!read_statement())
			return error("read_statement failed.");
	}

	if (!read(TEND))
		return error("'end' is not found.");

	return NORMAL;
}

static int read_statement() {
	if (is_assignment_statement()) {
		if (!read_assignment_statement())
			return error("read_assignment_statement failed.");

	} else if (is_condition_statement()) {
		if (!read_condition_statement())
			return error("read_condition_statement failed.");

	} else if (is_iteration_statement()) {
		if (!read_iteration_statement())
			return error("read_iteration_statement failed.");

	} else if (is_exit_statement()) {
		if (!read_exit_statement())
			return error("read_exit_statement failed.");

	} else if (is_call_statement()) {
		if (!read_call_statement())
			return error("read_call_statement failed.");

	} else if (is_return_statement()) {
		if (!read_return_statement())
			return error("read_return_statement failed.");

	} else if (is_input_statement()) {
		if (!read_input_statement())
			return error("read_input_statement failed.");

	} else if (is_output_statement()) {
		if (!read_output_statement())
			return error("read_output_statement failed.");

	} else if (is_compound_statement()) {
		if (!read_compound_statement())
			return error("read_compound_statement failed.");

	} else
		if (!read_empty_statement())
			return error("read_empty_statement failed."); // {unreachable}

	return NORMAL;
}

static int is_condition_statement() {
	return is(TIF);
}

static int read_condition_statement() {
	if (!read(TIF))
		return error("'if' is not found.");

	if (!read_expression())
		return error("read_expression failed.");

	if (!read(TTHEN))
		return error("'then' is not found.");

	if (!read_statement())
		return error("read_statement failed.");

	if (is(TELSE)) {
		if (!read(TELSE))
			return error("'else' is not found.");

		if (!read_statement())
			return error("read_statement failed.");
	}

	return NORMAL;
}

static int is_iteration_statement() {
	return is(TWHILE);
}

static int read_iteration_statement() {
	if (!read(TWHILE))
		return error("'while' is not found.");

	if (!read_expression())
		return error("read_expression failed.");

	if (!read(TDO))
		return error("'do' is not found.");

	if (!read_statement())
		return error("read_statement failed.");

	return NORMAL;
}

static int is_exit_statement() {
	return is(TBREAK);
}

static int read_exit_statement() {
	if (!read(TBREAK))
		return error("'break' is not found.");
	
	return NORMAL;
}

static int is_call_statement() {
	return is(TCALL);
}

static int read_call_statement() {
	if (!read(TCALL))
		return error("'call' is not found.");

	if (!read(TNAME))
		return error("callee procedure name not found.");

	if (is(TLPAREN)) {
		if (!read(TLPAREN))
			return error("'(' is not found.");

		if (!read_expressions())
			return error("read_expressions not found.");

		if (!read(TRPAREN))
			return error("')' is not found.");
	}

	return NORMAL;
}

static int read_expressions() {
	if (!read_expression())
		return error("read_expression failed.");

	while (is(TCOMMA)) {
		if (!read(TCOMMA))
			return error("',' is not found.");

		if (!read_expression())
			return error("read_expression failed.");
	}

	return NORMAL;
}

static int is_return_statement() {
	return is(TRETURN);
}

static int read_return_statement() {
	if (!read(TRETURN))
		return error("'return' is not found.");

	return NORMAL;
}

static int is_assignment_statement() {
	return is_left_part();
}

static int read_assignment_statement() {
	if (!read_left_part())
		return error("read_left_part failed.");

	if (!read(TASSIGN))
		return error("':=' is not found.");

	if (!read_expression())
		return error("read_expression failed.");

	return NORMAL;
}

static int is_left_part() {
	return is_variable();
}

static int read_left_part() {
	if (!read_variable())
		return error("read_variable failed.");

	return NORMAL;
}

static int is_variable() {
	return is_variable_name();
}

static int read_variable() {
	if (!read_variable_name())
		return error("read_variable_name failed.");		

	if (is(TLSQPAREN)) {
		if (!read(TLSQPAREN))
			return error("'[' is not found.");		

		if (!read_expressions())
			return error("read_expressions failed.");		

		if (!read(TRSQPAREN))
			return error("']' is not found.");
	}

	return NORMAL;
}

static int is_expression() {
	return is_simple_expression();
}

static int read_expression() {
	if (!read_simple_expression())
		return error("read_simple_expression failed.");		

	while (is_relational_operator()) {
		if (!read_relational_operator())
			return error("read_relational_operator failed.");		

		if (!read_simple_expression())
			return error("read_simple_expression failed.");
	}

	return NORMAL;
}

static int is_simple_expression() {
	return is(TPLUS) || is(TMINUS) || is_term();
}

static int read_simple_expression() {
	if (is(TPLUS))
		if (!read(TPLUS))
			return error("'+' is not found.");		

	if (is(TMINUS))
		if (!read(TMINUS))
			return error("'-' is not found.");		

	if (!read_term())
		return error("read_term failed.");		

	while (is_additive_operator()) {
		if (!read_additive_operator())
			return error("read_additive_operator failed.");

		if (!read_term())
			return error("read_term failed.");
	}

	return NORMAL;
}

static int is_term() {
	return is_factor();
}

static int read_term() {
	if (!read_factor())
		return error("read_factor failed.");

	while (is_multiplicative_operator()) {
		if (!read_multiplicative_operator())
			return error("read_multiplicative_operator failed.");		

		if (!read_factor())
			return error("read_factor failed.");		
	}

	return NORMAL;
}

static int is_factor() {
	return is_variable() || is_constant() ||
		is(TLPAREN) || is(TNOT) || is_standard_type();
}

static int read_factor() {
	if (is_variable())
		if (!read_variable())
			return error("read_variable failed.");		
		else
			return NORMAL;

	if (is_constant())
		if (!read_constant())
			return error("read_constant failed.");		
		else
			return NORMAL;

	if (is(TLPAREN)) {
		if (!read(TLPAREN))
			return error("'(' is not found.");		
		if (!read_expression())
			return error("read_expression failed.");
		if (!read(TRPAREN))
			return error("')' is not found.");
		else
			return NORMAL;
	}

	if (is(TNOT)) {
		if (!read(TNOT))
			return error("'not' is not found.");		
		if (!read_factor())
			return error("read_factor failed.");		
		else
			return NORMAL;
	}

	if (is_standard_type()) {
		if (!read_standard_type())
			return error("read_standard_type failed.");		
		if (!read(TLPAREN))
			return error("'(' is not found.");		
		if (!read_expression())
			return error("read_expression failed.");
		if (!read(TRPAREN))
			return error("')' is not found.");
		else
			return NORMAL;
	}

	return error("unmachted on read_factor.");
}

static int is_constant() {
	return is(TNUMBER) || is(TFALSE) || is(TTRUE) || is(TSTRING);
}

static int read_constant() {
	if (is(TNUMBER))
		if (!read(TNUMBER))
			return error("number is not found.");
		else
			return NORMAL;

	if (is(TFALSE))
		if (!read(TFALSE))
			return error("'false' is not found.");
		else
			return NORMAL;

	if (is(TTRUE))
		if (!read(TTRUE))
			return error("'true' is not found.");
		else
			return NORMAL;

	if (is(TSTRING))
		if (!read(TSTRING))
			return error("string is not found.");
		else
			return NORMAL;

	return error("unmachted on read_constant.");
}

static int is_multiplicative_operator() {
	return is(TSTAR) || is(TDIV) || is(TAND);
}

static int read_multiplicative_operator() {
	if (is(TSTAR)) {
		if (!read(TSTAR))
			return error("'*' is not found.");
		return NORMAL;
	}

	if (is(TDIV)) {
		if (!read(TDIV))
			return error("'div' is not found.");
		return NORMAL;
	}

	if (is(TAND)) {
		if (!read(TAND))
			return error("'and' is not found.");
		return NORMAL;
	}

	return error("unmatched on read_multiplicative_operator.");	
}

static int is_additive_operator() {
	return is(TPLUS) || is(TMINUS) || is(TOR);
}

static int read_additive_operator() {
	if (is(TPLUS)) {
		if (!read(TPLUS))
			return error("'+' is not found.");
		return NORMAL;
	}

	if (is(TMINUS)) {
		if (!read(TMINUS))
			return error("'-' is not found.");
		return NORMAL;
	}

	if (is(TOR)) {
		if (!read(TOR))
			return error("'or' is not found.");
		return NORMAL;
	}

	return error("unmatched on read_additive_operator.");	
}

static int is_relational_operator() {
	return is(TEQUAL) || is(TNOTEQ)
		|| is(TLE) || is(TLEEQ)
		|| is(TGR) || is(TGREQ);
}

static int read_relational_operator() {
	if (is(TEQUAL)) {
		if (!read(TEQUAL))
			return error("'=' is not found.");
		return NORMAL;
	}

	if (is(TNOTEQ)) {
		if (!read(TNOTEQ))
			return error("'<>' is not found.");
		return NORMAL;
	}

	if (is(TLE)) {
		if (!read(TLE))
			return error("'<' is not found.");
		return NORMAL;
	}

	if (is(TLEEQ)) {
		if (!read(TLEEQ))
			return error("'<=' is not found.");
		return NORMAL;
	}

	if (is(TGR)) {
		if (!read(TGR))
			return error("'>' is not found.");
		return NORMAL;
	}

	if (is(TGREQ)) {
		if (!read(TGREQ))
			return error("'>=' is not found.");
		return NORMAL;
	}

	return error("unmatched on read_relational_operator.");
}

static int is_input_statement() {
	return is(TREAD) || is(TREADLN);
}

static int read_input_statement() {
	if (is(TREAD)) {
		if (!read(TREAD))
			return error("'read' is not found.");
	} else if (is(TREADLN)) {
		if (!read(TREADLN))
			return error("'readln' is not found.");
	} else
		return error("unmatched on read_input_statement.");

	if (is(TLPAREN)) {
		if (!read(TLPAREN))
			return error("'(' is not found.");

		if (!read_variable())
			return error("read_variable failed.");

		while (is(TCOMMA)) {
			if (!read(TCOMMA))
				return error("',' is not found.");
			if (!read_variable())
				return error("read_variable failed.");
		}

		if (!read(TRPAREN))
			return error("')' is not found.");
	}

	return NORMAL;
}

static int is_output_statement() {
	return is(TWRITE) || is(TWRITELN);
}

static int read_output_statement() {
	if (is(TWRITE)) {
		if (!read(TWRITE))
			return error("'write' is not found.");
	} else if (is(TWRITELN)) {
		if (!read(TWRITELN))
			return error("'writeln' is not found.");
	} else
		return error("unmatched on read_output_statement.");

	if (is(TLPAREN)) {
		if (!read(TLPAREN))
			return error("'(' is not found.");

		if (!read_output_format())
			return error("read_output_format failed.");

		while (is(TCOMMA)) {
			if (!read(TCOMMA))
				return error("',' is not found.");
			if (!read_output_format())
				return error("read_output_format failed.");
		}

		if (!read(TRPAREN))
			return error("')' is not found.");
	}

	return NORMAL;
}

static int read_output_format() {
	if (is_expression()) {
		if (!read_expression())
			return error("read_expression failed.");

		if (is(TCOLON)) {
			if (!read(TCOLON))
				return error("':' is not found.");
			if (!read(TNUMBER))
				return error("number is not found.");
		}

	} else if (is(TSTRING))
		if (!read(TSTRING))
			return error("string is not found.");
	else
		return error("unmatched on read_output_format.");

	return NORMAL;
}

static int read_empty_statement() {
	return NORMAL;
}
