#include "Parser.h"

static int token;

typedef struct {
	int code;
	int num;
	char str[MAXSTRSIZE];
} tset;

tset token_prev = { 0, 0, "" };
tset token_prev_prev = { 0, 0, "" };
tset token_prev_prev_prev = { 0, 0, "" };

static int newline = 1;

static void update_token() {
	/* print */
	// update
	token_prev_prev_prev = token_prev_prev;
	token_prev_prev = token_prev;

	token_prev.code = token;
	token_prev.num = num_attr;
	strcpy(token_prev.str, string_attr);

	if (token_prev_prev.code != 0) {
		// whitespace
		if (!newline && token_prev_prev.code != TSEMI
			&& token_prev_prev.code != TCOMMA
			&& token_prev_prev.code != TLPAREN
			&& token_prev_prev.code != TRPAREN
			&& token_prev_prev.code != TLSQPAREN
			&& token_prev_prev.code != TRSQPAREN
			&& token_prev_prev_prev.code != TLPAREN
			&& token_prev_prev_prev.code != TLSQPAREN)
			printf(" ");

		// print token
		if (token_prev_prev.code == TNUMBER) {
			printf("%d", token_prev_prev.num);
		} else if (token_prev_prev.code == TSTRING) {
			printf("'%s'", token_prev_prev.str);
		} else if (token_prev_prev.code == TNAME) {
			printf("%s", token_prev_prev.str);
		} else if (token_prev_prev.code == TELSE && !newline) {
			printf("\n%s", tokencode_to_str(token_prev_prev.code));
		} else if (token_prev_prev.code == TEND && !newline) {
			printf("\n%s", tokencode_to_str(token_prev_prev.code));
		} else {
			printf("%s", tokencode_to_str(token_prev_prev.code));
		}

		newline = 0;
		if (token_prev_prev.code == TSEMI
			|| token_prev_prev.code == TBEGIN
			|| token_prev_prev.code == TTHEN
			|| token_prev_prev.code == TELSE && token_prev.code != TBEGIN && token_prev.code != TIF) {
			printf("\n");
			newline = 1;
		}
	}
	/* - */

	token = scan();
}

static int error(char *message) {
	if (!newline) {
		printf("\n\n");
		newline = 1;
	}

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

extern int parse_program() {
	update_token();

	int err = NORMAL;

	if (!read(TPROGRAM)) {
		err = error("'program' is not found.");
		goto PRINT_RET;
	}

	if (!read(TNAME)) {
		err = error("program name is not found.");
		goto PRINT_RET;
	}

	if (!read(TSEMI)) {
		err = error("';' is not found.");
		goto PRINT_RET;
	}

	if (!read_block()) {
		err = error("read_block failed.");
		goto PRINT_RET;
	}

	if (!read(TDOT)) {
		err = error("'.' is not found.");
		goto PRINT_RET;
	}

	printf("%s", tokencode_to_str(token_prev.code)); // {redundant: .}
	return NORMAL;

	PRINT_RET:
	printf("                subsequent tokens: [%s] [%s]\n", tokencode_to_str(token_prev.code), tokencode_to_str(token));
	return err;
}

static int read_block() {
	while (is_variable_declaration() || is_subprogram_declaration()) {
		if (is_variable_declaration())
			if (!read_variable_declaration())
				return error("read_variable_declaration failed.");

		if (is_subprogram_declaration())
			if (!read_subprogram_declaration())
				return error("read_subprogram_declaration failed.");
	}

	if (!read_compound_statement())
		return error("read_compound_statement failed.");

	return NORMAL;
}

static int is_variable_declaration() {
	return is(TVAR);
}

static int read_variable_declaration() {
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

	while (is_variable_names()) {
		if (!read_variable_names())
			return error("read_variable_names failed.");

		if (!read(TCOLON))
			return error("':' is not found.");

		if (!read_type())
			return error("read_type failed.");

		if (!read(TSEMI))
			return error("';' is not found.");
	}

	return NORMAL;
}

static int is_variable_names() {
	return is_variable_name();
}

static int read_variable_names() {
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
	if (!read(TNAME))
		return error("variable name is not found.");
}

static int read_type() {
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

static int read_array_type() {
	if (!read(TARRAY))
		return error("'array' is not found.");

	if (!read(TLSQPAREN))
		return error("'[' is not found.");

	if (!read(TNUMBER))
		return error("number is not found.");

	if (!read(TRSQPAREN))
		return error("']' is not found.");

	if (!read(TOF))
		return error("'of' is not found.");

	if (!read_standard_type())
		return error("read_standard_type failed.");

	return NORMAL;
}

static int is_subprogram_declaration() {
	return is(TPROCEDURE);
}

static int read_subprogram_declaration() {
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

	if (!read_compound_statement())
		return error("read_compound_statement failed.");

	if (!read(TSEMI))
		return error("';' is not found.");

	return NORMAL;
}

static int read_procedure_name() {
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

	while (is(TSEMI)) {
		if (!read(TSEMI))
			return error("';' is not found.");

		if (!read_variable_names())
			return error("read_variable_names failed.");

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
