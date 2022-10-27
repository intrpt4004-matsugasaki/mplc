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

static int parse(const int TOKEN_CODE) {
	const int match = (token == TOKEN_CODE);
	update_token();

	return match;
}

extern int parse_program() {
	update_token();

	if (!parse(TPROGRAM))
		return error("Keyword 'program' is not found.");

	if (!parse(TNAME))
		return error("");

	if (!parse(TSEMI))
		return error("");

	if (!parse_block())
		return error("");

	if (!parse(TDOT))
		return error("");

	return NORMAL;
}

static int parse_block() {
	while (is_variable_declaration() || is_subprogram_declaration()) {
		if (is_variable_declaration())
			if (!parse_variable_declaration())
				return error("");

		if (is_subprogram_declaration())
			if (!parse_subprogram_declaration())
				return error("");
	}

	if (!parse_compound_statement())
		reutrn error("");

	return NORMAL;
}

static int is_variable_declaration() {
	return token == TVAR;
}

static int parse_variable_declaration() {
	if (!parse(TVAR))
		return error("");

	if (!parse_variable_names())
		return error("");

	if (!parse(TCOLON))
		return error("");

	if (!parse_type())
		return error("");

	if (!parse(TSEMI))
		return error("");

	while (1) {
		if (is_variable_names())
			if (!parse_variable_names())
				return error("");
	}
}

static int is_subprogram_declaration() {
	return token == TPROCEDURE;
}

static int parse_subprogram_declaration() {
}

static int parse_compound_statement() {

}

static int parse_output_format() {
	if (!parse_expression())
		return error("");
	
	// TODO:
}

static int parse_empty_statement() {
	return NORMAL;
}
