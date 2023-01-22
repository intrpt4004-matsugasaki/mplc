#include "TypeAllocator.h"

static void allocate_type_in_constant(constant_t *cons) {
	if (cons->kind == NUMBER) {
		cons->TYPE.kind = STANDARD;
		cons->TYPE.standard = INTEGER;

	} else if (cons->kind == FALSE || cons->kind == TRUE) {
		cons->TYPE.kind = STANDARD;
		cons->TYPE.standard = BOOLEAN;

	} else if (cons->kind == STRING) {
		cons->TYPE.kind = ARRAY;
		cons->TYPE.array.elem_type = CHAR;
		cons->TYPE.array.size = strlen(cons->string);
	}
}

static void allocate_type_in_variable_indicator(program_t *program, REF_SCOPE scope, variable_indicator_t *var_idr) {
	if (var_idr->is_array)
		allocate_type_in_expression(program, scope, var_idr->index);

	if (scope.kind == SCOPE_PROGRAM) {
		for (variable_t *v = program->var; v != NULL; v = v->next) {
			if (!strcmp(v->name, var_idr->name)) {
				var_idr->TYPE = v->type;
				return;
			}
		}

	} else if (scope.kind == SCOPE_PROCEDURE) {
		for (procedure_t *p = program->proc; p != NULL; p = p->next) {
			if (!strcmp(p->name, scope.proc_name)) {
				for (variable_t *pp = p->param; pp != NULL; pp = pp->next) {
					if (!strcmp(pp->name, var_idr->name)) {
						var_idr->TYPE = pp->type;
						return;
					}
				}

				for (variable_t *pv = p->var; pv != NULL; pv = pv->next) {
					if (!strcmp(pv->name, var_idr->name)) {
						var_idr->TYPE = pv->type;
						return;
					}
				}
			}
		}
	}
}

static void allocate_type_in_factor(program_t *program, REF_SCOPE scope, factor_t *factor) {
	if (factor->kind == VAR_IDR) {
		allocate_type_in_variable_indicator(program, scope, &factor->var_idr);

	} else if (factor->kind == CONST) {
		allocate_type_in_constant(&factor->cons);
		factor->TYPE = factor->cons.TYPE;

	} else if (factor->kind == EXPR) {
		allocate_type_in_expression(program, scope, factor->expr);
		factor->TYPE = factor->expr->TYPE;

	} else if (factor->kind == INVERT_FACTOR) {
		allocate_type_in_factor(program, scope, factor->next);
		factor->TYPE = factor->next->TYPE;

	} else if (factor->kind == STD_TYPE) {
		factor->TYPE.kind = STANDARD;
		factor->TYPE.standard = factor->std_type;
	}
}

static void allocate_type_in_term(program_t *program, REF_SCOPE scope, term_t *term) {
	allocate_type_in_factor(program, scope, &term->factor);
	term->TYPE = term->factor.TYPE;

	if (term->next != NULL)
		allocate_type_in_term(program, scope, term->next);
}

static void allocate_type_in_simple_expression(program_t *program, REF_SCOPE scope, simple_expression_t *simp_expr) {
	allocate_type_in_term(program, scope, &simp_expr->term);
	simp_expr->TYPE = simp_expr->term.TYPE;

	if (simp_expr->next != NULL)
		allocate_type_in_simple_expression(program, scope, simp_expr->next);
}

static void allocate_type_in_expression(program_t *program, REF_SCOPE scope, expression_t *expr) {
	allocate_type_in_simple_expression(program, scope, &expr->simp_expr);

	if (expr->next != NULL)
		allocate_type_in_expression(program, scope, expr->next);
}

static void allocate_type_in_expressions(program_t *program, REF_SCOPE scope, expressions_t *exprs) {
	for (expressions_t *ep = exprs; ep != NULL; ep = ep->next) {
		allocate_type_in_expression(program, scope, &ep->expr);
	}
}

static void allocate_type_in_variable_indicators(program_t *program, REF_SCOPE scope, variable_indicators_t *var_idrs) {
	for (variable_indicators_t *ip = var_idrs; ip != NULL; ip = ip->next) {
		allocate_type_in_variable_indicator(program, scope, &ip->var_idr);
	}
}

static void allocate_type_in_output_format(program_t *program, REF_SCOPE scope, output_format_t *fmt) {
	if (fmt->kind == EXPR_MODE) {
		allocate_type_in_expression(program, scope, &fmt->expr);
		fmt->TYPE = fmt->expr.TYPE;
	}
	else if (fmt->kind == STR_MODE) {
		fmt->TYPE.kind = ARRAY;
		fmt->TYPE.array.elem_type = CHAR;
		fmt->TYPE.array.size = strlen(fmt->string);
	}
}

static void allocate_type_in_output_formats(program_t *program, REF_SCOPE scope, output_formats_t *fmts) {
	for (output_formats_t *fp = fmts; fp != NULL; fp = fp->next) {
		allocate_type_in_output_format(program, scope, &fmts->format);
	}
}

static void allocate_type_in_statement(program_t *program, REF_SCOPE scope, statement_t *stmt) {
	if (stmt == NULL)
		return;

	if (stmt->kind == ASSIGN) {
		assignment_statement_t *s1 = (assignment_statement_t *)(stmt);

		allocate_type_in_variable_indicator(program, scope, &s1->target_var_idr);
		allocate_type_in_expression(program, scope, &s1->expr);

	} else if (stmt->kind == CONDITION) {
		condition_statement_t *s1 = (condition_statement_t *)(stmt);

		allocate_type_in_expression(program, scope, &s1->cond);
		allocate_type_in_statement(program, scope, s1->then_stmt);

		if (s1->has_else_stmt)
			allocate_type_in_statement(program, scope, s1->else_stmt);

	} else if (stmt->kind == ITERATION) {
		iteration_statement_t *s1 = (iteration_statement_t *)(stmt);

		allocate_type_in_expression(program, scope, &s1->cond);
		allocate_type_in_statement(program, scope, s1->loop_stmt);

	} else if (stmt->kind == CALL) {
		call_statement_t *s1 = (call_statement_t *)(stmt);

		allocate_type_in_expressions(program, scope, s1->param);

	} else if (stmt->kind == INPUT) {
		input_statement_t *s1 = (input_statement_t *)(stmt);

		allocate_type_in_variable_indicators(program, scope, s1->target_var_idrs);

	} else if (stmt->kind == OUTPUT) {
		output_statement_t *s1 = (output_statement_t *)(stmt);

		allocate_type_in_output_formats(program, scope, s1->formats);
	}

	allocate_type_in_statement(program, scope, stmt->next);
}

static void allocate_type_in_procedure(program_t *program, procedure_t *proc) {
	proc->ARITY = 0;
	for (variable_t *var = proc->param; var != NULL; var = var->next)
		proc->ARITY++;

	proc->TYPE = malloc(proc->ARITY * sizeof(standard_type_t));

	int i = 0;
	for (variable_t *var = proc->param; var != NULL; var = var->next) {
		proc->TYPE[i++] = var->type;
	}
}

extern void allocate_type(program_t *program) {
	// program block
	REF_SCOPE scope;
	scope.kind = SCOPE_PROGRAM;
	allocate_type_in_statement(program, scope, program->stmt);

	// procedure block
	for (procedure_t *p = program->proc; p != NULL; p = p->next) {
		allocate_type_in_procedure(program, p);

		REF_SCOPE scope;
		scope.kind = SCOPE_PROCEDURE;
		strcpy(scope.proc_name, p->name);

		allocate_type_in_statement(program, scope, p->stmt);
	}
}
