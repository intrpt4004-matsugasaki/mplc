#include "Debug.h"

extern char *token_code_s(const token_code_t token_code) {
	switch (token_code) {
		case TNAME:			return "NAME";
		case TPROGRAM:		return "program";
		case TVAR:			return "var";
		case TARRAY:		return "array";
		case TOF:			return "of";
		case TBEGIN:		return "begin";
		case TEND:			return "end";
		case TIF:			return "if";
		case TTHEN:			return "then";
		case TELSE:			return "else";
		case TPROCEDURE:	return "procedure";
		case TRETURN:		return "return";
		case TCALL:			return "call";
		case TWHILE:		return "while";
		case TDO:			return "do";
		case TNOT:			return "not";
		case TOR:			return "or";
		case TDIV:			return "div";
		case TAND:			return "and";
		case TCHAR:			return "char";
		case TINTEGER:		return "integer";
		case TBOOLEAN:		return "boolean";
		case TREADLN:		return "readln";
		case TWRITELN:		return "writeln";
		case TTRUE:			return "true";
		case TFALSE:		return "false";
		case TNUMBER:		return "NUMBER";
		case TSTRING:		return "STRING";
		case TPLUS:			return "+";
		case TMINUS:		return "-";
		case TSTAR:			return "*";
		case TEQUAL:		return "=";
		case TNOTEQ:		return "<>";
		case TLE:			return "<";
		case TLEEQ:			return "<=";
		case TGR:			return ">";
		case TGREQ:			return ">=";
		case TLPAREN:		return "(";
		case TRPAREN:		return ")";
		case TLSQPAREN:		return "[";
		case TRSQPAREN:		return "]";
		case TASSIGN:		return ":=";
		case TDOT:			return ".";
		case TCOMMA:		return ",";
		case TCOLON:		return ":";
		case TSEMI:			return ";";
		case TREAD:			return "read";
		case TWRITE:		return "write";
		case TBREAK:		return "break";
		default:			return "{ERROR}";
	}
}

extern char *standard_type_s(const standard_type_t std_type) {
	switch (std_type) {
		case INTEGER:		return "integer";
		case BOOLEAN:		return "boolean";
		case CHAR:			return "char";
		default:			return "{ERROR}";
	}
}

extern char *type_s(const type_t type) {
	if (type.kind == STANDARD)
		return standard_type_s(type.standard);

	char *tmp = malloc(sizeof(char) * MAXSTRSIZE);
	sprintf(tmp, "array[%d] of %s", type.array.size, standard_type_s(type.array.elem_type));
	return tmp;
}

extern char *multiplicative_operator_s(const multiplicative_operator_t mul_opr) {
	switch (mul_opr) {
		case ASTERISK:		return "*";
		case DIV:			return "div";
		case AND:			return "and";
		default:			return "{ERROR}";
	}
}

extern char *additive_operator_s(const additive_operator_t add_opr) {
	switch (add_opr) {
		case PLUS:			return "+";
		case MINUS:			return "-";
		case OR:			return "or";
		default:			return "{ERROR}";
	}
}

extern char *relational_operator_s(const relational_operator_t rel_opr) {
	switch (rel_opr) {
		case EQUAL:				return "=";
		case NOT_EQUAL:			return "<>";
		case LESS:				return "<";
		case LESS_OR_EQUAL:		return "<=";
		case GREATER:			return ">";
		case GREATER_OR_EQUAL:	return ">=";
		default:				return "{ERROR}";
	}
}

extern char *boolean_s(const constant_t cons) {
	return (cons.kind == FALSE) ? "FALSE" : "TRUE";
}

extern void print_constant(const constant_t cons) {
	if (cons.kind == NUMBER) { // nazeka switch deha ugokanai.
		printf("%d", cons.number);
	}
	else if (cons.kind == TRUE) {
		printf("true");
	}
	else if (cons.kind == FALSE) {
		printf("false");
	}
	else if (cons.kind == STRING) {
		printf("%s", cons.string);
	}
}

extern void print_variable_indicator(const variable_indicator_t var_idr) {
	printf("%s", var_idr.name);

	if (var_idr.is_array) {
		printf("[");
		print_expression(*var_idr.index);
		printf("]");
	}
}

extern void print_factor(const factor_t factor) {
	if (factor.kind == VAR_IDR) { // nazeka switch deha ugokanai.
		print_variable_indicator(factor.var_idr);
	}
	else if (factor.kind == CONST) {
		print_constant(factor.cons);
	}
	else if (factor.kind == EXPR) {
		print_expression(*factor.expr);
	}
	else if (factor.kind == INVERT_FACTOR) {
		printf("!");
		print_factor(*factor.inv_factor);
	}
	else if (factor.kind == CAST_EXPR) {
		printf("(%s)", standard_type_s(factor.cast_std_type));
		print_expression(*factor.cast_expr);
	}
}

extern void print_term(const term_t term) {
	print_factor(term.factor);

	if (term.next != NULL) {
		printf(" %s ", multiplicative_operator_s(term.mul_opr));
		print_term(*term.next);
	}
}

extern void print_simple_expression(const simple_expression_t simp_expr) {
	print_term(simp_expr.term);

	if (simp_expr.next != NULL) {
		printf(" %s ", additive_operator_s(simp_expr.add_opr));
		print_simple_expression(*simp_expr.next);
	}
}

extern void print_expression(const expression_t expr) {
	print_simple_expression(expr.simp_expr);

	if (expr.next != NULL) {
		printf(" %s ", relational_operator_s(expr.rel_opr));
		print_expression(*expr.next);
	}
}
