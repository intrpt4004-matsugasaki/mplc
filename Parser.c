#include "Parser.h"

static int token;

static void update_token() {
	token = scan();
}

static int error(char *message) {
	printf("ERROR: %s\n", message);
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

extern int parse_program() {
	update_token();

	if (!read(TPROGRAM))
		return error("Keyword 'program' is not found.");

	if (!read(TNAME))
		return error("{}");

	if (!read(TSEMI))
		return error("';' is not found.");

	if (!read_block())
		return error("{}");

	if (!read(TDOT))
		return error("'.' is not found.");

	return NORMAL;
}

static int read_block() {
	while (is_variable_declaration() || is_subprogram_declaration()) {
		if (is_variable_declaration())
			if (!read_variable_declaration())
				return error("");

		if (is_subprogram_declaration())
			if (!read_subprogram_declaration())
				return error("");
	}

	if (!read_compound_statement())
		reutrn error("");

	return NORMAL;
}

static int is_variable_declaration() {
	return is(TVAR);
}

static int read_variable_declaration() {
	if (!read(TVAR))
		return error("");

	if (!read_variable_names())
		return error("");

	if (!read(TCOLON))
		return error("");

	if (!read_type())
		return error("");

	if (!read(TSEMI))
		return error("");

	while (is_variable_names()) {
		if (!read_variable_names())
			return error("");

		if (!read(TCOLON))
			return error("");

		if (!read_type())
			return error("");

		if (!read(TSEMI))
			return error("");
	}

	return NORMAL;
}

static int read_variable_names() {
	if (!read_variable_name())
		return error("");

	while (is(TCOLON)) {
		if (!read(TCOLON))
			return error("");

		if (!read_variable_name())
			return error("");
	}
		
}

static int read_variable_name() {
	if (!read(TNAME))
		return error("");
}

static int read_type() {
	if (is_standard_type()) {
		if (!read_standard_type())
			return error("");
		return NORMAL;
	}
	
	if (is_array_type()) {
		if (!read_array_type())
			return error("");
		return NORMAL;
	}

	return error("[mismatch] type");
}

static int is_standard_type() {
	return is(TINTEGER) || is(TBOOLEAN) || is(TCHAR);
}

static int read_standard_type() {
	if (is(TINTEGER)) {
		if (!read(TINTEGER))
			return error("");
		return NORMAL;
	}

	if (is(TBOOLEAN)) {
		if (!read(TBOOLEAN))
			return error("");
		return NORMAL;
	}

	if (is(TCHAR)) {
		if (!read(TCHAR))
			return error("");
		return NORMAL;
	}

	return error("[mismatch] standard_type");
}

static int is_array_type() {
	return is(TARRAY);
}

static int read_array_type() {
	if (!read(TARRAY))
		return error("");

	if (!read(TLSQPAREN))
		return error("");

	if (!read(TNUMBER))
		return error("");

	if (!read(TRSQPAREN))
		return error("");

	if (!read(TOF))
		return error("");

	if (!read_standard_type())
		return error("");

	return NORMAL;
}

static int is_subprogram_declaration() {
	return is(TPROCEDURE);
}

static int read_subprogram_declaration() {
	if (!read(TPROCEDURE))
		return error("");

	if (!read_procedure_name())
		return error("");

	if (is_formal_parameters())
		if (!read_formal_parameters())
			return error("");

	if (!read(TSEMI))
		return error("");

	if (is_variable_declaration())
		if (!read_variable_declaration())
			return error("");

	if (!read_compound_statement())
		return error("");

	if (!read(TSEMI))
		return error("");

	return NORMAL;
}

static int read_procedure_name() {
	if (!read(TNAME))
		return error("");
	
	return NORMAL;
}

static read_formal_parameters() {
	if (!read(TLPAREN))
		return error("");

	if (!read_variable_names())
		return error("");

	if (!read(TCOLON))
		return error("");

	if (!read_type())
		return error("");

	while (is(TSEMI)) {
		if (!read(TSEMI))
			return error("");

		if (!read_variable_names())
			return error("");

		if (!read(TCOLON))
			return error("");

		if (!read_type())
			return error("");
	}

	if (!read(TRPAREN))
		return error("");

	return NORMAL;
}

static int read_compound_statement() {
	if (!read(TBEGIN))
		return error("");

	if (!read_statement())
		return error("");

	while (is(TSEMI)) {
		if (!read(TSEMI))
			return error("");

		if (!read_statement())
			return error("");
	}

	if (!read(TEND))
		return error("");

	return NORMAL;
}

static int read_statement() {
	if (is_assignment_statement())
		if (!read_assignment_statement())
			reutrn error("");
		else
			return NORMAL;

	if (is_condition_statement())
		if (!read_condition_statement())
			reutrn error("");
		else
			return NORMAL;

	if (is_iteration_statement())
		if (!read_iteration_statement())
			reutrn error("");
		else
			return NORMAL;

	if (is_exit_statement())
		if (!read_exit_statement())
			reutrn error("");
		else
			return NORMAL;

	if (is_call_statement())
		if (!read_call_statement())
			reutrn error("");
		else
			return NORMAL;

	if (is_return_statement())
		if (!read_return_statement())
			reutrn error("");
		else
			return NORMAL;

	if (is_input_statement())
		if (!read_input_statement())
			reutrn error("");
		else
			return NORMAL;

	if (is_output_statement())
		if (!read_output_statement())
			reutrn error("");
		else
			return NORMAL;

	if (is_compound_statement())
		if (!read_compound_statement())
			reutrn error("");
		else
			return NORMAL;

	if (!read_empty_statement())
		return error("");
	else
		return NORMAL;

	return error("[mismatch] statement"); // {unreachable}
}

static int is_condition_statement() {
	return is(TIF);
}

static int read_condition_statement() {
	if (!read(TIF))
		return error("");

	if (!read_expression())
		return error("");

	if (!read(TTHEN))
		return error("");

	if (!read_statement())
		return error("");

	if (is(TELSE)) {
		if (!read(TELSE))
			return error("");
		if (!read_statement())
			return error("");
	}

	return NORMAL;
}

static int is_iteration_statement() {
	return is(TWHILE);
}

static int read_iteration_statement() {
	if (!read(TWHILE))
		return error("");

	if (!read_expression())
		return error("");

	if (!read(TDO))
		return error("");

	if (!read_statement())
		return error("");

	return NORMAL;
}

static int is_exit_statement() {
	return is(TBREAK);
}

static int read_exit_statement() {
	if (!read(TBREAK))
		reutrn error("");
	
	return NORMAL;
}

// todo

static int additive_operator() {
	if (is(TPLUS)) {
		if (!read(TPLUS))
			return error("");
		return NORMAL;
	}

	if (is(TMINUS)) {
		if (!read(TMINUS))
			return error("");
		return NORMAL;
	}

	if (is(TOR)) {
		if (!read(TOR))
			return error("");
		return NORMAL;
	}

	return error("[mismatch] additive operator");
}

static int read_relational_operator() {
	if (is(TEQUAL)) {
		if (!read(TEQUAL))
			return error("");
		return NORMAL;
	}

	if (is(TNOTEQ)) {
		if (!read(TNOTEQ))
			return error("");
		return NORMAL;
	}

	if (is(TLE)) {
		if (!read(TLE))
			return error("");
		return NORMAL;
	}

	if (is(TLEEQ)) {
		if (!read(TLEEQ))
			return error("");
		return NORMAL;
	}

	if (is(TGR)) {
		if (!read(TGR))
			return error("");
		return NORMAL;
	}

	if (is(TGREQ)) {
		if (!read(TGREQ))
			return error("");
		return NORMAL;
	}

	return error("[mismatch] relational operator");
}

// todo

static int read_output_format() {
	if (!read_expression())
		return error("");
	
	// TODO:
}

static int read_empty_statement() {
	return NORMAL;
}
