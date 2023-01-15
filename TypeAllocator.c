#include "TypeAllocator.h"

static void allocate_type_in_constant(constant_t *cons) {
	if (cons->kind == NUMBER) {
		cons->TYPE = INTEGER;
	} else if (cons->kind == FALSE || cons->kind == TRUE) {
		cons->TYPE = BOOLEAN;
	} else if (cons->kind == STRING) {
		cons->TYPE = STRING;
	}
}

/*
static void variable_declared(program_t program, REF_SCOPE scope, variable_indicator_t var_idr) {
	variable_t *v;
	for (v = program.var; v != NULL; v = v->next) {
		if (!strcmp(v->name, var_idr.name)) {
			return;
		}
	}

	if (scope.kind == SCOPE_PROCEDURE) {
		procedure_declared(program, scope.proc_name); // hitsuyou?

		for (procedure_t *p = program.proc; p != NULL; p = p->next) {
			if (!strcmp(p->name, scope.proc_name)) {
				for (variable_t *pp = p->param; pp != NULL; pp = pp->next) {
					if (!strcmp(pp->name, var_idr.name)) {
						return;
					}
				}

				for (variable_t *pv = p->var; pv != NULL; pv = pv->next) {
					if (!strcmp(pv->name, var_idr.name)) {
						return;
					}
				}
			}
		}
	}

	char tmp[MAXSTRSIZE];
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

	char tmp[MAXSTRSIZE];
	sprintf(tmp, "undeclared procedure: %s", proc_name);
	error(tmp);
}

static void name_declared_in_factor(program_t program, REF_SCOPE scope, factor_t factor) {
	if (factor.kind == VAR_IDR) {
		name_declared_in_variable_indicator(program, scope, factor.var_idr);
	} else if (factor.kind == EXPR) {
		allocate_type_in_expression(program, scope, *factor.expr);
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

static void allocate_type_in_expression(program_t program, REF_SCOPE scope, expression_t expr) {
	name_declared_in_simple_expression(program, scope, expr.simp_expr);

	if (expr.next != NULL)
		allocate_type_in_expression(program, scope, *expr.next);
}

static void allocate_type_in_expressions(program_t program, REF_SCOPE scope, expressions_t *exprs) {
	for (expressions_t *ep = exprs; ep != NULL; ep = ep->next) {
		allocate_type_in_expression(program, scope, ep->expr);
	}
}

static void name_declared_in_variable_indicator(program_t program, REF_SCOPE scope, variable_indicator_t var_idr) {
	variable_declared(program, scope, var_idr);

	if (var_idr.is_array)
		allocate_type_in_expression(program, scope, *var_idr.index);
}

static void name_declared_in_variable_indicators(program_t program, REF_SCOPE scope, variable_indicators_t *var_idrs) {
	for (variable_indicators_t *ip = var_idrs; ip != NULL; ip = ip->next) {
		name_declared_in_variable_indicator(program, scope, ip->var_idr);
	}
}

static void name_declared_output_formats(program_t program, REF_SCOPE scope, output_formats_t *fmts) {
	for (output_formats_t *fp = fmts; fp != NULL; fp = fp->next) {
		if (fp->format.kind = EXPR_MODE)
			allocate_type_in_expression(program, scope, fp->format.expr);
	}
}

static void allocate_type_in_statement(program_t *program, REF_SCOPE scope, statement_t *s) {
	if (s->kind == EXIT || s->kind == RETURN || s->kind == EMPTY)
		return;

	if (s->kind == ASSIGN) {
		assignment_statement_t *s1 = (assignment_statement_t *)(s);

		allocate_type_in_variable_indicator(program, scope, s1->target_var_idr);

		if (s1->target.is_array)
			allocate_type_in_expression(program, scope, *s1->target_var_idr.index);

		allocate_type_in_expression(program, scope, s1->expr);

	} else if (s->kind == CONDITION) {
		condition_statement_t *s1 = (condition_statement_t *)(s);

		allocate_type_in_expression(program, scope, s1->cond);
		allocate_type_in_statement(program, scope, s1->then_stmt);

		if (s1->has_else_stmt)
			allocate_type_in_statement(program, scope, s1->else_stmt);

	} else if (s->kind == ITERATION) {
		iteration_statement_t *s1 = (iteration_statement_t *)(s);

		allocate_type_in_expression(program, scope, s1->cond);
		allocate_type_in_statement(program, scope, s1->loop_stmt);

	} else if (s->kind == CALL) {
		call_statement_t *s1 = (call_statement_t *)(s);

		procedure_declared(program, s1->name);
		allocate_type_in_expressions(program, scope, s1->param);

	} else if (s->kind == INPUT) {
		input_statement_t *s1 = (input_statement_t *)(s);

		name_declared_in_variable_indicators(program, scope, s1->target);

	} else if (s->kind == OUTPUT) {
		output_statement_t *s1 = (output_statement_t *)(s);

		name_declared_output_formats(program, scope, s1->format);
	}
}
*/
static void allocate_type_in_procedure(program_t *program, procedure_t *proc) {
	int size = 0;
	for (variable_t *var = proc->param; var != NULL; var = var->next)
		size++;

	proc->PARAM_TYPE = malloc(size * sizeof(standard_type_t));

	int i = 0;
	for (variable_t *var = proc->param; var != NULL; var = var->next) {
		proc->PARAM_TYPE[i++] = var->type;
	}
}

extern void allocate_type(program_t *program) {
	// program block
	for (statement_t *s = program->stmt; s != NULL; s = s->next) {
		REF_SCOPE scope;
		scope.kind = SCOPE_PROGRAM;
//		allocate_type_in_statement(program, scope, s);
	}

	// procedure block
	for (procedure_t *p = program->proc; p != NULL; p = p->next) {
		allocate_type_in_procedure(program, p);

		REF_SCOPE scope;
		scope.kind = SCOPE_PROCEDURE;
		strcpy(scope.proc_name, p->name);

/*		for (statement_t *s = p->stmt; s != NULL; s = s->next) {
			allocate_type_in_statement(program, scope, s);
		}
*/	}
}
