#include "TypeAllocator.h"

static void allocate_type_in_constant(constant_t *cons) {
	if (cons->kind == NUMBER) {
		cons->TYPE.kind = STANDARD;
		cons->TYPE.standard = INTEGER;

		printf("%s", type_s(cons->TYPE));
	}
	else if (cons->kind == FALSE || cons->kind == TRUE) {
		cons->TYPE.kind = STANDARD;
		cons->TYPE.standard = BOOLEAN;

		printf("%s", type_s(cons->TYPE));
	}
	else if (cons->kind == STRING) {
		/* Extended specification */
		cons->TYPE.kind = ARRAY;
		cons->TYPE.array.elem_type = CHAR;
		cons->TYPE.array.size = strlen(cons->string);

		/* Standard specification */
		/*cons->TYPE.kind = STANDARD;
		cons->TYPE.standard = CHAR;*/

		printf("%s", type_s(cons->TYPE));
	}
}

static void allocate_type_in_variable_indicator(program_t *program, REF_SCOPE scope, variable_indicator_t *var_idr) {
	if (var_idr->is_array)
		allocate_type_in_expression(program, scope, var_idr->index);

	if (scope.kind == SCOPE_PROGRAM) {
		for (variable_t *v = program->var; v != NULL; v = v->next) {
			if (!strcmp(v->name, var_idr->name)) {
				var_idr->TYPE = v->type;

				printf("%s", type_s(var_idr->TYPE));
				return;
			}
		}
	}
	else if (scope.kind == SCOPE_PROCEDURE) {
		for (procedure_t *p = program->proc; p != NULL; p = p->next) {
			if (!strcmp(p->name, scope.proc_name)) {
				for (variable_t *pp = p->param; pp != NULL; pp = pp->next) {
					if (!strcmp(pp->name, var_idr->name)) {
						var_idr->TYPE = pp->type;

						printf("%s", type_s(var_idr->TYPE));
						return;
					}
				}

				for (variable_t *pv = p->var; pv != NULL; pv = pv->next) {
					if (!strcmp(pv->name, var_idr->name)) {
						var_idr->TYPE = pv->type;

						printf("%s", type_s(var_idr->TYPE));
						return;
					}
				}
			}
		}

		for (variable_t *v = program->var; v != NULL; v = v->next) {
			if (!strcmp(v->name, var_idr->name)) {
				var_idr->TYPE = v->type;

				printf("%s", type_s(var_idr->TYPE));
				return;
			}
		}
	}
}

static void allocate_type_in_factor(program_t *program, REF_SCOPE scope, factor_t *factor) {
	if (factor->kind == VAR_IDR) {
		allocate_type_in_variable_indicator(program, scope, &factor->var_idr);
		factor->TYPE = factor->var_idr.TYPE;

		printf(":%s(factor)", type_s(factor->TYPE));
	}
	else if (factor->kind == CONST) {
		allocate_type_in_constant(&factor->cons);
		factor->TYPE = factor->cons.TYPE;

		printf(":%s(factor)", type_s(factor->TYPE));
	}
	else if (factor->kind == EXPR) {
		allocate_type_in_expression(program, scope, factor->expr);
		factor->TYPE = factor->expr->TYPE;

		printf(":%s(factor)", type_s(factor->TYPE));
	}
	else if (factor->kind == INVERT_FACTOR) {
		allocate_type_in_factor(program, scope, factor->inv_factor);
		factor->TYPE = factor->inv_factor->TYPE;

		printf(":%s(factor)", type_s(factor->TYPE));
	}
	else if (factor->kind == CAST_EXPR) {
		printf("(%s)(", type_s(factor->TYPE));

		factor->TYPE.kind = STANDARD;
		factor->TYPE.standard = factor->cast_std_type;

		printf(")");
	}
}

static void allocate_type_in_term(program_t *program, REF_SCOPE scope, term_t *term) {
	allocate_type_in_factor(program, scope, &term->factor);
	term->TYPE = term->factor.TYPE;
	printf(":%s(term)", type_s(term->TYPE));

	if (term->next != NULL) {
		printf(" %s ", multiplicative_operator_s(term->mul_opr));

		allocate_type_in_term(program, scope, term->next);
	}
}

static void allocate_type_in_simple_expression(program_t *program, REF_SCOPE scope, simple_expression_t *simp_expr) {
	allocate_type_in_term(program, scope, &simp_expr->term);
	simp_expr->TYPE = simp_expr->term.TYPE;
	printf(":%s(simp_expr)", type_s(simp_expr->TYPE));

	if (simp_expr->next != NULL) {
		printf(" %s ", additive_operator_s(simp_expr->add_opr));

		allocate_type_in_simple_expression(program, scope, simp_expr->next);
	}
}

static void allocate_type_in_expression(program_t *program, REF_SCOPE scope, expression_t *expr) {
	expr->is_start_point = 1;
	allocate_type_in_simple_expression(program, scope, &expr->simp_expr);

	if (expr->next != NULL) {
		printf(" %s ", relational_operator_s(expr->rel_opr));
		allocate_type_in_expression(program, scope, expr->next);
		expr->next->is_start_point = 0;

		expr->TYPE = expr->simp_expr.TYPE;
		expr->WHOLE_TYPE.kind = STANDARD;
		expr->WHOLE_TYPE.standard = BOOLEAN;
	} else {
		expr->TYPE = expr->simp_expr.TYPE;
		expr->WHOLE_TYPE = expr->TYPE;
	}

	printf(":%s(expr)", type_s(expr->TYPE));
	printf(":%s(expr')", type_s(expr->WHOLE_TYPE));
	printf("\n");
	print_expression(*expr);
	printf("\n");
	fflush(stdout);
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
		//printf("'%s'", fmt->string);
	}

	//printf(":%s (ofmt)\n", type_s(fmt->TYPE));
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
		printf(" := ");
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

	proc->TYPE = malloc(proc->ARITY * sizeof(type_t));

	int i = 0;
	for (variable_t *var = proc->param; var != NULL; var = var->next) {
		proc->TYPE[i++] = var->type;
	}
}

extern void allocate_type(program_t *program) {
	// procedure block
	for (procedure_t *p = program->proc; p != NULL; p = p->next) {
		allocate_type_in_procedure(program, p);

		REF_SCOPE scope;
		scope.kind = SCOPE_PROCEDURE;
		strcpy(scope.proc_name, p->name);

		allocate_type_in_statement(program, scope, p->stmt);
		printf("\n\n\n");
	}

	// program block
	REF_SCOPE scope;
	scope.kind = SCOPE_PROGRAM;
	allocate_type_in_statement(program, scope, program->stmt);
}
