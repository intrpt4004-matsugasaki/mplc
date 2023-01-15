#include "SemanticAnalyzer.h"

// ---
static apr_file aprf[100];
static int aprf_length = 0;
// ---

static void error(char *message) {
	/* ERROR */
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
				sprintf(tmp, "overloaded name: %s (at line %d)", v->name, v->DEF_LINE_NUM);
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
					sprintf(tmp, "overloaded name: %s (at line %d)", v->name, v->DEF_LINE_NUM);
					error(tmp);
				}
			}
		}

		for (variable_t *v = p->param; v != NULL; v = v->next) {
			for (variable_t *w = p->var; w != NULL; w = w->next) {
				if (!strcmp(v->name, w->name)) {
					char tmp[MAXSTRSIZE];
					sprintf(tmp, "overloaded name: %s (at line %d)", v->name, v->DEF_LINE_NUM);
					error(tmp);
				}
			}
		}
	}
}

static void variable_declared(program_t program, REF_SCOPE scope, variable_indicator_t var_idr) {
	variable_t *v;
	for (v = program.var; v != NULL; v = v->next) {
		if (!strcmp(v->name, var_idr.name)) {
			// ---
			aprf_store(v->name, var_idr.APR_LINE_NUM);
			// ---
			return;
		}
	}

	if (scope.kind == SCOPE_PROCEDURE) {
		procedure_declared(program, scope.proc_name); // hitsuyou?

		for (procedure_t *p = program.proc; p != NULL; p = p->next) {
			if (!strcmp(p->name, scope.proc_name)) {
				for (variable_t *pp = p->param; pp != NULL; pp = pp->next) {
					if (!strcmp(pp->name, var_idr.name)) {
						// ---
						static char tmp[MAXSTRSIZE];
						sprintf(tmp, "%s:%s", pp->name, scope.proc_name);
						aprf_store(tmp, var_idr.APR_LINE_NUM);
						// ---
						return;
					}
				}

				for (variable_t *pv = p->var; pv != NULL; pv = pv->next) {
					if (!strcmp(pv->name, var_idr.name)) {
						// ---
						static char tmp[MAXSTRSIZE];
						sprintf(tmp, "%s:%s", pv->name, scope.proc_name);
						aprf_store(tmp, var_idr.APR_LINE_NUM);
						// ---
						return;
					}
				}
			}
		}
	}

	static char tmp[MAXSTRSIZE];
	sprintf(tmp, "undeclared variable: %s (at line %d)", var_idr.name, var_idr.APR_LINE_NUM);
	error(tmp);
}

static void procedure_declared(program_t program, char *proc_name) {
	procedure_t *p;
	for (p = program.proc; p != NULL; p = p->next) {
		if (!strcmp(p->name, proc_name)) {
			return;
		}
	}

	static char tmp[MAXSTRSIZE];
	sprintf(tmp, "undeclared procedure: %s", proc_name);
	error(tmp);
}

static void name_declared_in_factor(program_t program, REF_SCOPE scope, factor_t factor) {
	if (factor.kind == VAR_IDR) {
		name_declared_in_variable_indicator(program, scope, factor.var_idr);
	} else if (factor.kind == EXPR) {
		name_declared_in_expression(program, scope, *factor.expr);
	} else if (factor.kind == INVERT_FACTOR) {
		name_declared_in_factor(program, scope, *factor.next);
	}
}

static void name_declared_in_term(program_t program, REF_SCOPE scope, term_t term) {
	name_declared_in_factor(program, scope, term.factor);

	if (term.next != NULL)
		name_declared_in_term(program, scope, *term.next);
}

static void name_declared_in_simple_expression(program_t program, REF_SCOPE scope, simple_expression_t simp_expr) {
	name_declared_in_term(program, scope, simp_expr.term);

	if (simp_expr.next != NULL)
		name_declared_in_simple_expression(program, scope, *simp_expr.next);
}

static void name_declared_in_expression(program_t program, REF_SCOPE scope, expression_t expr) {
	name_declared_in_simple_expression(program, scope, expr.simp_expr);

	if (expr.next != NULL)
		name_declared_in_expression(program, scope, *expr.next);
}

static void name_declared_in_expressions(program_t program, REF_SCOPE scope, expressions_t *exprs) {
	for (expressions_t *ep = exprs; ep != NULL; ep = ep->next) {
		name_declared_in_expression(program, scope, ep->expr);
	}
}

static void name_declared_in_variable_indicator(program_t program, REF_SCOPE scope, variable_indicator_t var_idr) {
	variable_declared(program, scope, var_idr);

	if (var_idr.is_array)
		name_declared_in_expression(program, scope, *var_idr.index);
}

static void name_declared_in_variable_indicators(program_t program, REF_SCOPE scope, variable_indicators_t *var_idrs) {
	for (variable_indicators_t *ip = var_idrs; ip != NULL; ip = ip->next) {
		name_declared_in_variable_indicator(program, scope, ip->var_idr);
	}
}

static void name_declared_output_formats(program_t program, REF_SCOPE scope, output_formats_t *fmts) {
	for (output_formats_t *fp = fmts; fp != NULL; fp = fp->next) {
		if (fp->format.kind = EXPR_MODE)
			name_declared_in_expression(program, scope, fp->format.expr);
	}
}

static void name_declared_in_statement(program_t program, REF_SCOPE scope, statement_t *s) {
	if (s->kind == EXIT || s->kind == RETURN || s->kind == EMPTY)
		return;

	if (s->kind == ASSIGN) {
		assignment_statement_t *s1 = (assignment_statement_t *)(s);

		variable_declared(program, scope, s1->target_var_idr);

		if (s1->target_var_idr.is_array)
			name_declared_in_expression(program, scope, *s1->target_var_idr.index);

		name_declared_in_expression(program, scope, s1->expr);

	} else if (s->kind == CONDITION) {
		condition_statement_t *s1 = (condition_statement_t *)(s);

		name_declared_in_expression(program, scope, s1->cond);
		name_declared_in_statement(program, scope, s1->then_stmt);

		if (s1->has_else_stmt)
			name_declared_in_statement(program, scope, s1->else_stmt);

	} else if (s->kind == ITERATION) {
		iteration_statement_t *s1 = (iteration_statement_t *)(s);

		name_declared_in_expression(program, scope, s1->cond);
		name_declared_in_statement(program, scope, s1->loop_stmt);

	} else if (s->kind == CALL) {
		call_statement_t *s1 = (call_statement_t *)(s);

		procedure_declared(program, s1->name);
		// ---
		aprf_store(s1->name, s1->APR_LINE_NUM);
		// ---
		name_declared_in_expressions(program, scope, s1->param);

	} else if (s->kind == INPUT) {
		input_statement_t *s1 = (input_statement_t *)(s);

		name_declared_in_variable_indicators(program, scope, s1->target_var_idrs);

	} else if (s->kind == OUTPUT) {
		output_statement_t *s1 = (output_statement_t *)(s);

		name_declared_output_formats(program, scope, s1->formats);
	}
}

static void there_is_no_undeclared_name(program_t program) {
	// program block
	for (statement_t *s = program.stmt; s != NULL; s = s->next) {
		REF_SCOPE scope;
		scope.kind = SCOPE_PROGRAM;
		name_declared_in_statement(program, scope, s);
	}

	// procedure block
	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		REF_SCOPE scope;
		scope.kind = SCOPE_PROCEDURE;
		strcpy(scope.proc_name, p->name);

		for (statement_t *s = p->stmt; s != NULL; s = s->next) {
			name_declared_in_statement(program, scope, s);
		}
	}
}

extern void name_analyze(program_t program) {
	//---
	for (int i = 0; i < 100; i++) {
		aprf[i].nums_length = 0;
	}
	//---
	there_is_no_overloaded_name(program);
	there_is_no_undeclared_name(program);
}

extern void type_analyze(program_t program) {
}

static void aprf_store(char *name, int apr_line_num) {
	for (int i = 0; i < aprf_length; i++) {
		if (!strcmp(aprf[i].name, name)) {
			aprf[i].apr_line_nums[aprf[i].nums_length++] = apr_line_num;
			return;
		}
	}

	strcpy(aprf[aprf_length].name, name);
	aprf[aprf_length].apr_line_nums[0] = apr_line_num;
	aprf[aprf_length].nums_length = 1;
	aprf_length++;
}

static int aprf_search(char *name) {
	for (int i = 0; i < aprf_length; i++) {
		if (!strcmp(aprf[i].name, name)) {
			return i;
		}
	}
	return -1;
}

static void print_aprf_apr_line_nums(char *name) {
	int idx = aprf_search(name);
	if (idx == -1) {
		return;
	}

	for (int i = 0; i < aprf[idx].nums_length; i++) {
		printf("%d ", aprf[idx].apr_line_nums[i]);
	}
	printf("\b");
}

static char *standard_type_t_to_str(standard_type_t stdtype) {
	if (stdtype == INTEGER) return "integer"; // nazeka switch deha ugokanai.
	if (stdtype == BOOLEAN) return "boolean";
	if (stdtype == CHAR) return "char";
	return "{ERROR}";
}

static int print_type_and_get_length(type_t type) {
	if (type.kind == STANDARD) {
		printf("%s", standard_type_t_to_str(type.standard));
		return strlen(standard_type_t_to_str(type.standard));
	}
	else {
		printf("array[%d] of %s", type.array.size, standard_type_t_to_str(type.array.elem_type));

		int sizelen = 1;
		int size = type.array.size;
		while (size /= 10) sizelen++;

		return 11 + strlen(standard_type_t_to_str(type.array.elem_type)) + sizelen;
	}
}

extern void print_xref_table(program_t program) {
	for (int i = 0; i < aprf_length; i++) {
		printf("%s: ", aprf[i].name);
		for (int j = 0; j < aprf[i].nums_length; j++)
			printf("%d ", aprf[i].apr_line_nums[j]);
	}
	printf("\n");

	printf("NAME                  TYPE                  Def.   Ref.\n");
	printf("-------------------------------------------------------------------------\n");

	int whspl = 22;
	int whspl2 = 7;

	for (variable_t *v = program.var; v != NULL; v = v->next) {
		printf("%s", v->name);
		for (int i = 0; strlen(v->name) < whspl && i < whspl - strlen(v->name); i++) printf(" ");

		int len = print_type_and_get_length(v->type);
		for (int i = 0; len < whspl && i < whspl - len; i++) printf(" ");

		printf("%d", v->DEF_LINE_NUM);
		int sizelen = 1;
		int size = v->DEF_LINE_NUM;
		while (size /= 10) sizelen++;
		for (int i = 0; sizelen < whspl2 && i < whspl2 - sizelen; i++) printf(" ");

		print_aprf_apr_line_nums(v->name);
		printf("\n");
	}

	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		printf("%s", p->name);
		for (int i = 0; strlen(p->name) < whspl && i < whspl - strlen(p->name); i++) printf(" ");
	
		printf("procedure(");
		int len = 11;
		for (variable_t *pp = p->param; pp != NULL; pp = pp->next) {
			len += print_type_and_get_length(pp->type) + 2;
			printf(", ");
		}
		if (len != 11) {
			len -= 2;
			printf("\b\b");
		}
		printf(")");
		for (int i = 0; len < whspl && i < whspl - len; i++) printf(" ");

		printf("%d", p->DEF_LINE_NUM);
		int sizelen = 1;
		int size = p->DEF_LINE_NUM;
		while (size /= 10) sizelen++;
		for (int i = 0; sizelen < whspl2 && i < whspl2 - sizelen; i++) printf(" ");

		print_aprf_apr_line_nums(p->name);
		printf("\n");

		for (variable_t *pp = p->param; pp != NULL; pp = pp->next) {
			printf("%s:%s", pp->name, p->name);
			for (int i = 0; strlen(pp->name) + strlen(p->name) + 1 < whspl && i < whspl - strlen(pp->name) - strlen(p->name) - 1; i++) printf(" ");

			int len = print_type_and_get_length(pp->type);
			for (int i = 0; len < whspl && i < whspl - len; i++) printf(" ");

			printf("%d", pp->DEF_LINE_NUM);
			sizelen = 1;
			size = pp->DEF_LINE_NUM;
			while (size /= 10) sizelen++;
			for (int i = 0; sizelen < whspl2 && i < whspl2 - sizelen; i++) printf(" ");

			print_aprf_apr_line_nums(pp->name);
			printf("\n");
		}

		for (variable_t *pv = p->var; pv != NULL; pv = pv->next) {
			printf("%s:%s", pv->name, p->name);
			for (int i = 0; strlen(pv->name) + strlen(p->name) + 1 < whspl && i < whspl - strlen(pv->name) - strlen(p->name) - 1; i++) printf(" ");

			int len = print_type_and_get_length(pv->type);
			for (int i = 0; len < whspl && i < whspl - len; i++) printf(" ");

			printf("%d", pv->DEF_LINE_NUM);
			sizelen = 1;
			size = pv->DEF_LINE_NUM;
			while (size /= 10) sizelen++;
			for (int i = 0; sizelen < whspl2 && i < whspl2 - sizelen; i++) printf(" ");

			print_aprf_apr_line_nums(pv->name);
			printf("\n");
		}
	}
}
