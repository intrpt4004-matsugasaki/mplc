#include "SemanticAnalyzer.h"

static void error(char *message) {
	printf("[ERROR] SemanticAnalyzer: %s\n", message);
	exit(-1);
}

static void there_is_no_overloaded_name(program_t program) {
	// program block
	for (variable_t *v = program.var; v != NULL; v = v->next) {
		if (v->next == NULL)
			break;

		for (variable_t *w = v->next; w != NULL; w = w->next) {
			if (!strcmp(v->name, w->name)) {
				char tmp[MAXSTRSIZE];
				sprintf(tmp, "overloaded name: %s", v->name);
				error(tmp);
			}
		}
	}

	// procedure block
	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		for (variable_t *v = p->var; v != NULL; v = v->next) {
			for (variable_t *w = program.var; w != NULL; w = w->next) {
				if (!strcmp(v->name, w->name)) {
					char tmp[MAXSTRSIZE];
					sprintf(tmp, "overloaded name: %s", v->name);
					error(tmp);
				}
			}
		}

		for (variable_t *v = p->param; v != NULL; v = v->next) {
			for (variable_t *w = p->var; w != NULL; w = w->next) {
				if (!strcmp(v->name, w->name)) {
					char tmp[MAXSTRSIZE];
					sprintf(tmp, "overloaded name: %s", v->name);
					error(tmp);
				}
			}
		}
	}
}

static void variable_declared(program_t program, char *var_name) {
	variable_t *v;
	for (v = program.var; v != NULL; v = v->next) {
		if (!strcmp(v->name, var_name)) {
			return;
		}
	}

	char tmp[MAXSTRSIZE];
	sprintf(tmp, "undeclared variable: %s", var_name);
	error(tmp);
}

static void procedure_declared(program_t program, char *proc_name) {
	procedure_t *p;
	for (p = program.proc; p != NULL; p = p->next) {
		if (!strcmp(p->name, proc_name)) {
			return;
		}
	}

	char tmp[MAXSTRSIZE];
	sprintf(tmp, "undeclared procedure: %s", proc_name);
	error(tmp);
}

static void name_declared_in_expression(program_t program, expression_t expr) {
	printf("expr\n");
}

static void name_declared_in_expressions(program_t program, expressions_t *exprs) {
	for (expressions_t *ep = exprs; ep != NULL; ep = ep->next) {
		name_declared_in_expression(program, ep->expr);
	}
}

static void name_declared_in_variable_indicator(program_t program, variable_indicator_t idr) {
	variable_declared(program, idr.name);

	if (idr.is_array)
		name_declared_in_expression(program, *idr.index);
}

static void name_declared_in_variable_indicators(program_t program, variable_indicators_t *idrs) {
	for (variable_indicators_t *ip = idrs; ip != NULL; ip = ip->next) {
		name_declared_in_variable_indicator(program, ip->var);
	}
}

static void name_declared_output_formats(program_t program, output_formats_t *fmts) {
	for (output_formats_t *fp = fmts; fp != NULL; fp = fp->next) {
		if (fp->format.kind = EXPR_MODE)
			name_declared_in_expression(program, fp->format.expr);
	}
}

static void name_declared_in_statement(program_t program, statement_t *s) {
	if (s->kind == EXIT || s->kind == RETURN || s->kind == EMPTY)
		return;

	if (s->kind == ASSIGN) {
		assignment_statement_t *s1 = (assignment_statement_t *)(s);

		variable_declared(program, s1->target.name);

		if (s1->target.is_array)
			name_declared_in_expression(program, *s1->target.index);

		name_declared_in_expression(program, s1->expr);

	} else if (s->kind == CONDITION) {
		condition_statement_t *s1 = (condition_statement_t *)(s);

		name_declared_in_expression(program, s1->cond);
		name_declared_in_statement(program, s1->then_stmt);

		if (s1->has_else_stmt)
			name_declared_in_statement(program, s1->else_stmt);

	} else if (s->kind == ITERATION) {
		iteration_statement_t *s1 = (iteration_statement_t *)(s);

		name_declared_in_expression(program, s1->cond);
		name_declared_in_statement(program, s1->loop_stmt);

	} else if (s->kind == CALL) {
		call_statement_t *s1 = (call_statement_t *)(s);

		procedure_declared(program, s1->name);
		name_declared_in_expressions(program, s1->param);

	} else if (s->kind == INPUT) {
		input_statement_t *s1 = (input_statement_t *)(s);

		name_declared_in_variable_indicators(program, s1->target);

	} else if (s->kind == OUTPUT) {
		output_statement_t *s1 = (output_statement_t *)(s);

		name_declared_output_formats(program, s1->format);
	}
}

static void there_is_no_undeclared_name(program_t program) {
	// program block
	for (statement_t *s = program.stmt; s != NULL; s = s->next) {
		name_declared_in_statement(program, s);
	}

	// procedure block
}

static int is_variables_declared_in_procedure(program_t program, char *proc_name, char *var_name) {
	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		if (!strcmp(p->name, proc_name)) {
			return 0;
		}
	}
	return -1;
}

extern void semantic_analyze(program_t program) {
	there_is_no_overloaded_name(program);
	there_is_no_undeclared_name(program);

	print_xref_table(program);
}

static char *stdtype_to_str(standard_type_t stdtype) {
	if (stdtype == INTEGER) return "integer"; // nazeka switch deha ugokanai.
	if (stdtype == BOOLEAN) return "boolean";
	if (stdtype == CHAR) return "char";
	return "{ERROR}";
}

static int dump_type(type_t type) {
	if (type.kind == STANDARD) {
		printf("%s", stdtype_to_str(type.standard));
		return strlen(stdtype_to_str(type.standard));
	}
	else {
		printf("array[%d] of %s", type.array.size, stdtype_to_str(type.array.elem_type));

		int sizelen = 1;
		int size = type.array.size;
		while (size /= 10) sizelen++;

		return 11 + strlen(stdtype_to_str(type.array.elem_type)) + sizelen;
	}
}

static char *stmtkind_to_str(statement_t *stmt) {
	if (stmt->kind == ASSIGN) return "assign";
	if (stmt->kind == CONDITION) return "condition";
	if (stmt->kind == ITERATION) return "iteration";
	if (stmt->kind == EXIT) return "exit";
	if (stmt->kind == CALL) return "call";
	if (stmt->kind == RETURN) return "return";
	if (stmt->kind == INPUT) return "input";
	if (stmt->kind == OUTPUT) return "output";
	if (stmt->kind == EMPTY) return "empty";
	return "{ERROR}";
}

static void print_xref_table(program_t program) {
	printf("NAME                  TYPE                  Def.   Ref.\n");
	printf("-------------------------------------------------------------------------\n");

	int whspl = 22;

	for (variable_t *v = program.var; v != NULL; v = v->next) {
		printf("%s", v->name);
		for (int i = 0; strlen(v->name) < whspl && i < whspl - strlen(v->name); i++) printf(" ");

		int len = dump_type(v->type);
		for (int i = 0; len < whspl && i < whspl - len; i++) printf(" ");

		printf("%d", v->_DEF_LINE_NUM);

		printf("\n");
	}

	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		printf("%s", p->name);
		for (int i = 0; strlen(p->name) < whspl && i < whspl - strlen(p->name); i++) printf(" ");
	
		printf("procedure(");
		int len = 11;
		for (variable_t *pp = p->param; pp != NULL; pp = pp->next) {
			len += dump_type(pp->type) + 2;
			printf(", ");
		}
		if (len != 11) {
			len -= 2;
			printf("\b\b");
		}
		printf(")");
		for (int i = 0; len < whspl && i < whspl - len; i++) printf(" ");

		printf("%d", p->_DEF_LINE_NUM);
		printf("\n");

		for (variable_t *pp = p->param; pp != NULL; pp = pp->next) {
			printf("%s:%s", pp->name, p->name);
			for (int i = 0; strlen(pp->name) + strlen(p->name) + 1 < whspl && i < whspl - strlen(pp->name) - strlen(p->name) - 1; i++) printf(" ");

			int len = dump_type(pp->type);
			for (int i = 0; len < whspl && i < whspl - len; i++) printf(" ");

			printf("\n");
		}

		for (variable_t *pv = p->var; pv != NULL; pv = pv->next) {
			printf("%s:%s", pv->name, p->name);
			for (int i = 0; strlen(pv->name) + strlen(p->name) + 1 < whspl && i < whspl - strlen(pv->name) - strlen(p->name) - 1; i++) printf(" ");

			int len = dump_type(pv->type);
			for (int i = 0; len < whspl && i < whspl - len; i++) printf(" ");

			printf("%d", pv->_DEF_LINE_NUM);

			printf("\n");
		}
	}
}
