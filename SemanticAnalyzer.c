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
		for (variable_t *v = p->param; v != NULL; v = v->next) {
			for (variable_t *w = program.var; w != NULL; w = w->next) {
				if (!strcmp(v->name, w->name)) {
					char tmp[MAXSTRSIZE];
					sprintf(tmp, "overloaded name: %s", v->name);
					error(tmp);
				}
			}
		}

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

static void inspect_declared_variable(program_t program, char *var_name) {
	for (variable_t *v = program.var; v != NULL; v = v->next) {
		if (!strcmp(v->name, var_name)) {
			char tmp[MAXSTRSIZE];
			sprintf(tmp, "undeclared variable: %s", v->name);
			error(tmp);
		}
	}
}

static void inspect_declared_procedure(program_t program, char *proc_name) {
	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		if (!strcmp(p->name, proc_name)) {
			char tmp[MAXSTRSIZE];
			sprintf(tmp, "undeclared procedure: %s", p->name);
			error(tmp);
		}
	}
}

static void inspect_declared_variable_in_expression(program_t program, expression_t expr) {
	//
}

static void inspect_declared_variable_in_statement(program_t program, statement_t *stmt) {
	//
}

static void inspect_declared_variable_in_expressions(program_t program, expressions_t *exprs) {
	//
}

static void inspect_declared_variable_in_variable_indicators(program_t program, variable_indicators_t idrs) {
	//
}

static void inspect_declared_variable_output_formats(program_t program, output_formats_t fmt) {
	//
}

static void there_is_no_undeclared_name(program_t program) {
	// program block
	for (statement_t *s = program.stmt; s != NULL; s = s->next) {
		if (s->kind == EXIT || s->kind == RETURN || s->kind == EMPTY)
			break;

		if (s->kind == ASSIGN) {
			assignment_statement_t *s1 = (assignment_statement_t *)(s);

			inspect_declared_variable(program, s1->target.name);
			inspect_declared_variable_in_expression(program, *s1->target.index);
			inspect_declared_variable_in_expression(program, s1->expr);

		} else if (s->kind == CONDITION) {
			condition_statement_t *s1 = (condition_statement_t *)(s);

			inspect_declared_variable_in_expression(program, s1->cond);
			inspect_declared_variable_in_statement(program, s1->then_stmt);

			if (s1->has_else_stmt)
				inspect_declared_variable_in_statement(program, s1->else_stmt);

		} else if (s->kind == ITERATION) {
			iteration_statement_t *s1 = (iteration_statement_t *)(s);

			inspect_declared_variable_in_expression(program, s1->cond);
			inspect_declared_variable_in_statement(program, s1->loop_stmt);

		} else if (s->kind == CALL) {
			call_statement_t *s1 = (call_statement_t *)(s);

			inspect_declared_procedure(program, s1->name);
			inspect_declared_variable_in_expressions(program, s1->param);

		} else if (s->kind == INPUT) {
			input_statement_t *s1 = (input_statement_t *)(s);

			inspect_declared_variable_in_variable_indicators(program, *s1->target);

		} else if (s->kind == OUTPUT) {
			output_statement_t *s1 = (output_statement_t *)(s);

			inspect_declared_variable_output_formats(program, *s1->format);
		}
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

//	print_xref_table(program);
}

static char *stdtype_to_str(standard_type_t stdtype) {
	if (stdtype == INTEGER) return "integer"; // nazeka switch deha ugokanai.
	if (stdtype == BOOLEAN) return "boolean";
	if (stdtype == CHAR) return "char";
	return "{ERROR}";
}

static void dump_type(type_t type) {
	if (type.kind == STANDARD) {
		printf("%s", stdtype_to_str(type.standard));
	}
	else
		printf("%s[%d]", stdtype_to_str(type.array.elem_type), type.array.size);
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
	printf("\n\n");
	printf("program: %s\n", program.name);

	for (variable_t *v = program.var; v != NULL; v = v->next) {
		printf("   ");
		dump_type(v->type);
		printf(" %s\n", v->name);
	}

	printf("\n");
	for (statement_t *s = program.stmt; s != NULL; s = s->next) {
		printf("   %s\n", stmtkind_to_str(s));
	}
	printf("\n");

	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		printf("   %s( ", p->name);

		for (variable_t *pp = p->param; pp != NULL; pp = pp->next) {
			dump_type(pp->type);
			printf(" %s; ", pp->name);
		}
		printf(")\n");

		for (variable_t *pv = p->var; pv != NULL; pv = pv->next) {
			printf("      ");
			dump_type(pv->type);
			printf(" %s\n", pv->name);
		}

		printf("\n");
		for (statement_t *ps = p->stmt; ps != NULL; ps = ps->next) {
			printf("      %s\n", stmtkind_to_str(ps));
		}
		printf("\n");
	}
}
