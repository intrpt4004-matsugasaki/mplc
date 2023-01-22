#include "CodeGenerator.h"

static int Lcnt = 0;

extern char *get_stem(char *filename) {
	char *stem = malloc(sizeof(char) * MAXSTRSIZE);
	strcpy(stem, filename);

	int i = 0;
	for (; stem[i] != '\0'; i++);
	for (; stem[i] != '.'; i--);
	for (; stem[i] != '\0'; i++)
		stem[i] = '\0';

	return stem;
}

static FILE *fp;

static void dump(char *label_format, char *opcode /* or macro */, char *operand, char *comment, ...) {
	va_list ap;
	va_start(ap, comment);

	if (strlen(label_format) != 0) vfprintf(fp, label_format, ap);
	char tmp[MAXSTRSIZE];
	vsprintf(tmp, label_format, ap);
	for (int i = 0; 25 > strlen(tmp) && i < 25 - strlen(tmp); i++) fprintf(fp, " ");
	fprintf(fp, " ");

	va_end(ap);

	if (strlen(opcode) != 0) fprintf(fp, "%s", opcode);
	for (int i = 0; 10 > strlen(opcode) && i < 10 - strlen(opcode); i++) fprintf(fp, " ");
	fprintf(fp, " ");

	if (strlen(operand) != 0) fprintf(fp, "%s", operand);
	for (int i = 0; 15 > strlen(operand) && i < 15 - strlen(operand); i++) fprintf(fp, " ");
	fprintf(fp, " ");

	if (strlen(comment) != 0) fprintf(fp, "; %s", comment);

	fprintf(fp, "\n");
}

static void generate_code_constant(FILE *fp, constant_t cons) {
}

static void generate_code_variable_indicator(FILE *fp, variable_indicator_t var_idr) {
}

static void generate_code_factor(FILE *fp, factor_t factor) {
	if (factor.kind == VAR_IDR) {
		generate_code_variable_indicator(fp, factor.var_idr);
	}
	else if (factor.kind == CONST) {
		generate_code_constant(fp, factor.cons);
	}
	else if (factor.kind == EXPR) {
		generate_code_expression(fp, *factor.expr);
	}
	else if (factor.kind == INVERT_FACTOR) {
		generate_code_factor(fp, *factor.inv_factor);
	}
	else if (factor.kind == CAST_EXPR) {
		generate_code_expression(fp, *factor.cast_expr);
	}
}

static void generate_code_term(FILE *fp, term_t term) {
	generate_code_factor(fp, term.factor);

	if (term.next != NULL) {
		generate_code_term(fp, *term.next);
	}
}


static void generate_code_simple_expression(FILE *fp, simple_expression_t simp_expr) {
	generate_code_term(fp, simp_expr.term);

	if (simp_expr.next != NULL) {
		generate_code_simple_expression(fp, *simp_expr.next);
	}
}

static void generate_code_expression(FILE *fp, expression_t expr) {
	generate_code_simple_expression(fp, expr.simp_expr);

	if (expr.next != NULL) {
		generate_code_expression(fp, *expr.next);
	}
}

static void generate_code_statement(FILE *fp, statement_t *stmt) {
	if (stmt == NULL) return;

	char tmp[MAXSTRSIZE];

	if (stmt->kind == CALL) {
		call_statement_t *s1 = (call_statement_t *)(stmt);

		for (expressions_t *ep = s1->param; ep != NULL; ep = ep->next) {
			generate_code_expression(fp, ep->expr);
			dump("", "PUSH", "GR1", "");
		}
	
		sprintf(tmp, "$%s", s1->name);
		dump("", "CALL", tmp, "");
	}
	else if (stmt->kind == ITERATION) {
		iteration_statement_t *s1 = (iteration_statement_t *)(stmt);

		int l = Lcnt; Lcnt += 2;
		dump("#%d", "NOP", "", "", l);
		generate_code_expression(fp, s1->cond);
		dump("", "CPL", "GR1, ONE", "");
		sprintf(tmp, "#%d", l+1);
		dump("", "JZ", tmp, "");
		generate_code_statement(fp, s1->loop_stmt);
		sprintf(tmp, "#%d", l);
		dump("", "JUMP", tmp, "");
		dump("#%d", "NOP", tmp, "", l+1);
	}
	else if (stmt->kind == CONDITION) {
		condition_statement_t *s1 = (condition_statement_t *)(stmt);

		int l = Lcnt; Lcnt += 2;
		generate_code_expression(fp, s1->cond);
		dump("", "CPL", "GR1, ONE", "");
		sprintf(tmp, "#%d", l);
		dump("", "JZ", tmp, "");
		generate_code_statement(fp, s1->then_stmt);
		sprintf(tmp, "#%d", l+1);
		dump("", "JUMP", tmp, "");
		dump("#%d", "NOP", "", "", l);
		if (s1->has_else_stmt) {
			generate_code_statement(fp, s1->else_stmt);
		}
		dump("#%d", "NOP", "", "", l+1);
	}
	else if (stmt->kind == ASSIGN) {
		assignment_statement_t *s1 = (assignment_statement_t *)(stmt);

		generate_code_expression(fp, s1->expr);
		if (s1->target_var_idr.is_array) {
			//
		}
		sprintf(tmp, "GR1, %%%s", s1->target_var_idr.name);
		dump("", "ST", tmp, "");
	}
	else if (stmt->kind == EXIT) {
		// break loop
	}
	else if (stmt->kind == RETURN) {
		dump("", "RET", "", "");
	}
	else if (stmt->kind == INPUT) {
		//
	}
	else if (stmt->kind == OUTPUT) {
		//
	}

	generate_code_statement(fp, stmt->next);
}

static int get_variable_size(variable_t *v) {
	if (v->type.kind == STANDARD) return 1;
	return v->type.array.size;
}

extern void generate_code(char *filename, program_t program) {
	fp = fopen(filename, "w");
	Lcnt = 0;
	char tmp[MAXSTRSIZE];

	dump("$%s", "START", "$main", "", get_stem(filename));
	dump("", "", "", " ");

	// program block
	for (variable_t *v = program.var; v != NULL; v = v->next) {
		sprintf(tmp, "%d", get_variable_size(v));
		dump("%%%s", "DS", tmp, "", v->name);
	}

	// procedure block
	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		for (variable_t *v = p->var; v != NULL; v = v->next) {
			sprintf(tmp, "%d", get_variable_size(v));
			dump("%%%s.%s", "DS", tmp, "", p->name, v->name);
		}
	}
	dump("", "", "", " ");

	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		dump("$%s", "DS", "0", "", p->name);

//		for (variable_t *pp = p->param; pp != NULL; pp = pp->next) {
		dump("", "RPOP", "", ""); // parameter>8 ???: call stack
//		}

		generate_code_statement(fp, p->stmt);
		dump("", "RET", "", "", p->name);
	}
	dump("", "", "", " ");

	dump("$main", "DS", "0", "");
	generate_code_statement(fp, program.stmt);
	dump("", "END", "", "");

	fclose(fp);
}
