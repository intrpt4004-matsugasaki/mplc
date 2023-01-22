#include "SemanticAnalyzer.h"

// print xref table -----------------------------------------
static apr_file aprf[100];
static int aprf_length = 0;
// ----------------------------------------------------------

static void error(char *message) {
	/* ERROR */
	printf("[ERROR] SemanticAnalyzer: %s\n", message);
	exit(-1);
}

/* --- name analyze ------------------------------------------------------------------- */
// overloaded name ------------------------------------------
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
// ----------------------------------------------------------

// undeclared name ------------------------------------------
static void variable_declared(program_t program, REF_SCOPE scope, variable_indicator_t var_idr) {
	if (scope.kind == SCOPE_PROGRAM) {
		for (variable_t *v = program.var; v != NULL; v = v->next) {
			if (!strcmp(v->name, var_idr.name)) {
				// ---
				aprf_store(v->name, var_idr.APR_LINE_NUM);
				// ---
				return;
			}
		}
	} else if (scope.kind == SCOPE_PROCEDURE) {
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

static void name_declared_in_variable_indicator(program_t program, REF_SCOPE scope, variable_indicator_t var_idr) {
	variable_declared(program, scope, var_idr);

	if (var_idr.is_array)
		name_declared_in_expression(program, scope, *var_idr.index);
}

static void name_declared_in_factor(program_t program, REF_SCOPE scope, factor_t factor) {
	if (factor.kind == VAR_IDR) {
		name_declared_in_variable_indicator(program, scope, factor.var_idr);
	} else if (factor.kind == EXPR) {
		name_declared_in_expression(program, scope, *factor.expr);
	} else if (factor.kind == INVERT_FACTOR) {
		name_declared_in_factor(program, scope, *factor.inv_factor);
	} else if (factor.kind == CAST_EXPR) {
		name_declared_in_expression(program, scope, *factor.cast_expr);
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

static void name_declared_in_variable_indicators(program_t program, REF_SCOPE scope, variable_indicators_t *var_idrs) {
	for (variable_indicators_t *ip = var_idrs; ip != NULL; ip = ip->next) {
		name_declared_in_variable_indicator(program, scope, ip->var_idr);
	}
}

static void name_declared_output_format(program_t program, REF_SCOPE scope, output_format_t fmt) {
	if (fmt.kind == EXPR_MODE)
		name_declared_in_expression(program, scope, fmt.expr);
}

static void name_declared_output_formats(program_t program, REF_SCOPE scope, output_formats_t *fmts) {
	for (output_formats_t *fp = fmts; fp != NULL; fp = fp->next) {
		name_declared_output_format(program, scope, fp->format);
	}
}

static void name_declared_in_statement(program_t program, REF_SCOPE scope, statement_t *stmt) {
	if (stmt == NULL)
		return;

	if (stmt->kind == ASSIGN) {
		assignment_statement_t *s1 = (assignment_statement_t *)(stmt);

		variable_declared(program, scope, s1->target_var_idr);
		name_declared_in_expression(program, scope, s1->expr);

	} else if (stmt->kind == CONDITION) {
		condition_statement_t *s1 = (condition_statement_t *)(stmt);

		name_declared_in_expression(program, scope, s1->cond);
		name_declared_in_statement(program, scope, s1->then_stmt);

		if (s1->has_else_stmt)
			name_declared_in_statement(program, scope, s1->else_stmt);

	} else if (stmt->kind == ITERATION) {
		iteration_statement_t *s1 = (iteration_statement_t *)(stmt);

		name_declared_in_expression(program, scope, s1->cond);
		name_declared_in_statement(program, scope, s1->loop_stmt);

	} else if (stmt->kind == CALL) {
		call_statement_t *s1 = (call_statement_t *)(stmt);

		procedure_declared(program, s1->name);
		// ---
		aprf_store(s1->name, s1->APR_LINE_NUM);
		// ---
		name_declared_in_expressions(program, scope, s1->param);

	} else if (stmt->kind == INPUT) {
		input_statement_t *s1 = (input_statement_t *)(stmt);

		name_declared_in_variable_indicators(program, scope, s1->target_var_idrs);

	} else if (stmt->kind == OUTPUT) {
		output_statement_t *s1 = (output_statement_t *)(stmt);

		name_declared_output_formats(program, scope, s1->formats);
	}

	name_declared_in_statement(program, scope, stmt->next);
}

static void there_is_no_undeclared_name(program_t program) {
	// program block
	REF_SCOPE scope;
	scope.kind = SCOPE_PROGRAM;
	name_declared_in_statement(program, scope, program.stmt);

	// procedure block
	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		REF_SCOPE scope;
		scope.kind = SCOPE_PROCEDURE;
		strcpy(scope.proc_name, p->name);

		name_declared_in_statement(program, scope, p->stmt);
	}
}
// ----------------------------------------------------------

extern void name_analyze(program_t program) {
	//---
	for (int i = 0; i < 100; i++) {
		aprf[i].nums_length = 0;
	}
	//---
	there_is_no_overloaded_name(program);
	there_is_no_undeclared_name(program);
}
/* ------------------------------------------------------------------------------------ */

/* --- type analyze ------------------------------------------------------------------- */
// type matched ---------------------------------------------
static int type_eq(type_t t1, type_t t2) {
	if (t1.kind =! t2.kind) return 0;

	if (t1.kind == STANDARD) {
		return t1.standard == t2.standard;
	} else {
		return t1.array.elem_type == t2.array.elem_type && t1.array.size == t2.array.size;		
	}
}

static int type_is_std_of(type_t t, standard_type_t stdtype) {
	return t.kind == STANDARD && t.standard == stdtype;
}

static void type_matched_in_variable_indicator(variable_indicator_t var_idr) {
	if (var_idr.is_array)
		if (!((*var_idr.index).TYPE.kind == STANDARD
			&& (*var_idr.index).TYPE.standard == INTEGER)) {
			char tmp[MAXSTRSIZE];
			sprintf(tmp, "type error (at line %d)", var_idr.APR_LINE_NUM);
			error(tmp);
		}
}

static void type_matched_in_factor(factor_t factor) {
	if (factor.kind == VAR_IDR) {
		type_matched_in_variable_indicator(factor.var_idr);
	}
	else if (factor.kind == EXPR) {
		type_matched_in_expression(*factor.expr);
	}
	else if (factor.kind == INVERT_FACTOR) {
		type_matched_in_factor(*factor.inv_factor);
		if (!type_is_std_of((*factor.inv_factor).TYPE, BOOLEAN)) {
			char tmp[MAXSTRSIZE];
			sprintf(tmp, "type error [not BOOLEAN] (at line ?,291)");
			error(tmp);
		}
	}
	else if (factor.kind == CAST_EXPR) {
		type_matched_in_expression(*factor.cast_expr);

		if (!(
			type_is_std_of((*factor.inv_factor).TYPE, INTEGER)
			|| type_is_std_of((*factor.inv_factor).TYPE, BOOLEAN)
			|| type_is_std_of((*factor.inv_factor).TYPE, CHAR))) {
			char tmp[MAXSTRSIZE];
			sprintf(tmp, "type error [INTEGER, BOOLEAN, CHAR] (at line ?,300)");
			error(tmp);
		}
	}
}

static void type_matched_in_term(term_t term) {
	type_matched_in_factor(term.factor);

	if (term.next != NULL) {
		type_matched_in_term(*term.next);

		if (term.mul_opr == ASTERISK || term.mul_opr == DIV) {
			if (!(type_is_std_of(term.TYPE, INTEGER) && type_is_std_of((*term.next).TYPE, INTEGER))) {
				char tmp[MAXSTRSIZE];
				sprintf(tmp, "type error (at line ?,299)");
				error(tmp);
			}
		}
		else if (term.mul_opr == AND) {
			if (!(type_is_std_of(term.TYPE, BOOLEAN) && type_is_std_of((*term.next).TYPE, BOOLEAN))) {
				char tmp[MAXSTRSIZE];
				sprintf(tmp, "type error (at line ?,306)");
				error(tmp);
			}
		}
	}
}


static void type_matched_in_simple_expression(simple_expression_t simp_expr) {
	type_matched_in_term(simp_expr.term);

	if (simp_expr.prefix != NONE) {
		if (!type_is_std_of(simp_expr.TYPE, INTEGER)) {
			char tmp[MAXSTRSIZE];
			sprintf(tmp, "type error (at line ?,322)");
			error(tmp);
		}
	}

	if (simp_expr.next != NULL) {
		type_matched_in_simple_expression(*simp_expr.next);

		if (simp_expr.add_opr == PLUS || simp_expr.add_opr == MINUS) {
			if (!(type_is_std_of(simp_expr.TYPE, INTEGER) && type_is_std_of((*simp_expr.next).TYPE, INTEGER))) {
				char tmp[MAXSTRSIZE];
				sprintf(tmp, "type error (at line ?,331)");
				error(tmp);
			}
		}
		else if (simp_expr.add_opr == OR) {
			if (!(type_is_std_of(simp_expr.TYPE, BOOLEAN) && type_is_std_of((*simp_expr.next).TYPE, BOOLEAN))) {
				char tmp[MAXSTRSIZE];
				sprintf(tmp, "type error (at line ?,338)");
				error(tmp);
			}
		}
	}
}

static void type_matched_in_expression(expression_t expr) {
	type_matched_in_simple_expression(expr.simp_expr);

	if (expr.next != NULL) {
		type_matched_in_expression(*expr.next);

		if (!type_eq(expr.TYPE, (*expr.next).TYPE)) {
			char tmp[MAXSTRSIZE];
			sprintf(tmp, "type error [expr rel_opr expr] (at line %d)", expr.LINE_NUM);
			error(tmp);
		}
	}
}

static void type_matched_in_expressions(expressions_t *exprs, procedure_t *proc) { // call stmt only
	int caller_cnt = 0;
	for (expressions_t *ep = exprs; ep != NULL; ep = ep->next) {
		type_matched_in_expression(ep->expr);
		caller_cnt++;
	}

	if (proc->ARITY != caller_cnt) {
		char tmp[MAXSTRSIZE];
		sprintf(tmp, "type error [caller_cnt /= arity of proc] (at line ?,386)");
		error(tmp);
	}

	expressions_t *ep = exprs;
	int pc = 1;
	for (variable_t *v = proc->param; v != NULL; v = v->next) {
		if (!type_eq(v->type, ep->expr.TYPE)) {
			char tmp[MAXSTRSIZE];
			sprintf(tmp, "type error [parameter %d] (at line ?,386)", pc);
			error(tmp);
		}
		ep = ep->next;
		pc++;
	}
}

static void type_matched_in_variable_indicators(variable_indicators_t *var_idrs) { // in stmt only
	for (variable_indicators_t *ip = var_idrs; ip != NULL; ip = ip->next) {
		type_matched_in_variable_indicator(ip->var_idr);

		if (!(
			type_is_std_of(ip->var_idr.TYPE, INTEGER)
			|| type_is_std_of(ip->var_idr.TYPE, CHAR))) {
			char tmp[MAXSTRSIZE];
			sprintf(tmp, "type error [not INTEGER, CHAR] (at line ?,392)");
			error(tmp);
		}

	}
}

static void type_matched_in_output_format(output_format_t fmt) { // out stmt only
	if (fmt.kind == EXPR_MODE) {
		type_matched_in_expression(fmt.expr);

		if (!(
			type_is_std_of(fmt.expr.TYPE, INTEGER)
			|| type_is_std_of(fmt.expr.TYPE, BOOLEAN)
			|| type_is_std_of(fmt.expr.TYPE, CHAR))) {
			char tmp[MAXSTRSIZE];
			sprintf(tmp, "type error [not std type; output format] (at line %d)", fmt.LINE_NUM);
			error(tmp);
		}
	}
}

static void type_matched_in_output_formats(output_formats_t *fmts) { // out stmt only
	for (output_formats_t *fp = fmts; fp != NULL; fp = fp->next) {
		type_matched_in_output_format(fp->format);
	}
}

static void condition_type_is_boolean_in_condition_statement(condition_statement_t *cond_stmt) {
	if (!(cond_stmt->cond.TYPE.kind == STANDARD
		&& cond_stmt->cond.TYPE.standard == BOOLEAN)) {
		char tmp[MAXSTRSIZE];
		sprintf(tmp, "condition type is not boolean (at line %d)", cond_stmt->cond.LINE_NUM);
		error(tmp);
	}
}

static void condition_type_is_boolean_in_iteration_statement(iteration_statement_t *iter_stmt) {
	if (!(iter_stmt->cond.TYPE.kind == STANDARD
		&& iter_stmt->cond.TYPE.standard == BOOLEAN)) {
		char tmp[MAXSTRSIZE];
		sprintf(tmp, "condition type is not boolean (at line %d)", iter_stmt->cond.LINE_NUM);
		error(tmp);
	}
}

static program_t globl_program;

static void type_matched_in_statement(statement_t *stmt) {
	if (stmt == NULL)
		return;

	if (stmt->kind == ASSIGN) {
		assignment_statement_t *s1 = (assignment_statement_t *)(stmt);

		type_matched_in_variable_indicator(s1->target_var_idr);
		type_matched_in_expression(s1->expr);

		if (!type_eq(s1->target_var_idr.TYPE, s1->expr.TYPE)) {
			char tmp[MAXSTRSIZE];
			sprintf(tmp, "type error [lp := expr] (at line %d)", s1->expr.LINE_NUM);
			error(tmp);	
		}

	} else if (stmt->kind == CONDITION) {
		condition_statement_t *s1 = (condition_statement_t *)(stmt);

		condition_type_is_boolean_in_condition_statement(s1);

		type_matched_in_expression(s1->cond);
		type_matched_in_statement(s1->then_stmt);

		if (s1->has_else_stmt)
			type_matched_in_statement(s1->else_stmt);

	} else if (stmt->kind == ITERATION) {
		iteration_statement_t *s1 = (iteration_statement_t *)(stmt);

		condition_type_is_boolean_in_iteration_statement(s1);

		type_matched_in_expression(s1->cond);
		type_matched_in_statement(s1->loop_stmt);

	} else if (stmt->kind == CALL) {
		call_statement_t *s1 = (call_statement_t *)(stmt);

		procedure_t *p = globl_program.proc;
		for (; p != NULL; p = p->next) {
			if (!strcmp(s1->name, p->name)) {
				break;
			}
		}

		type_matched_in_expressions(s1->param, p);

	} else if (stmt->kind == INPUT) {
		input_statement_t *s1 = (input_statement_t *)(stmt);

		type_matched_in_variable_indicators(s1->target_var_idrs);

	} else if (stmt->kind == OUTPUT) {
		output_statement_t *s1 = (output_statement_t *)(stmt);

		type_matched_in_output_formats(s1->formats);
	}

	type_matched_in_statement(stmt->next);
}

static void array_size_is_GT_0_in_variable_declaration(program_t program) {
	for (variable_t *v = program.var; v != NULL; v = v->next) {
		if (v->type.kind == ARRAY) {
			if (v->type.array.size <= 0) {
				char tmp[MAXSTRSIZE];
				sprintf(tmp, "array size error (at line %d)", v->DEF_LINE_NUM);
				error(tmp);
			}
		}
	}

	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		for (variable_t *v = p->var; v != NULL; v = v->next) {
			if (v->type.kind == ARRAY) {
				if (v->type.array.size <= 0) {
					char tmp[MAXSTRSIZE];
					sprintf(tmp, "array size error (at line %d)", v->DEF_LINE_NUM);
					error(tmp);
				}
			}
		}

		for (variable_t *v = p->param; v != NULL; v = v->next) {
			if (v->type.kind == ARRAY) {
				if (v->type.array.size <= 0) {
					char tmp[MAXSTRSIZE];
					sprintf(tmp, "array size error (at line %d)", v->DEF_LINE_NUM);
					error(tmp);
				}
			}
		}
	}
}

static void type_matched(program_t program) {
	array_size_is_GT_0_in_variable_declaration(program);

	//type_matched_in_statement(program.stmt);

	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		//type_matched_in_statement(p->stmt);
	}
}
// ----------------------------------------------------------

extern void type_analyze(program_t program) {
	globl_program = program;
	type_matched(program);
}
/* ------------------------------------------------------------------------------------ */

/* print xref table ------------------------------------------------------------------- */
static void aprf_store(char *name, int apr_line_num) {
	int i = aprf_search(name);

	if (i != -1) {
		aprf[i].apr_line_nums[aprf[i].nums_length++] = apr_line_num;
	} else {
		strcpy(aprf[aprf_length].name, name);
		aprf[aprf_length].apr_line_nums[0] = apr_line_num;
		aprf[aprf_length].nums_length = 1;
		aprf_length++;
	}
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
	printf("NAME                  TYPE                                              Def.      Ref.\n");
	printf("--------------------------------------------------------------------------------------------------\n");

	int whsplnt = 22;
	int whspltd = 50;
	int whspldr = 10;

	for (variable_t *v = program.var; v != NULL; v = v->next) {
		printf("%s", v->name);
		for (int i = 0; strlen(v->name) < whsplnt && i < whsplnt - strlen(v->name); i++) printf(" ");

		int len = print_type_and_get_length(v->type);
		for (int i = 0; len < whspltd && i < whspltd - len; i++) printf(" ");

		printf("%d", v->DEF_LINE_NUM);
		int sizelen = 1;
		int size = v->DEF_LINE_NUM;
		while (size /= 10) sizelen++;
		for (int i = 0; sizelen < whspldr && i < whspldr - sizelen; i++) printf(" ");

		print_aprf_apr_line_nums(v->name);
		printf("\n");
	}

	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		printf("%s", p->name);
		for (int i = 0; strlen(p->name) < whsplnt && i < whsplnt - strlen(p->name); i++) printf(" ");
	
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
		for (int i = 0; len < whspltd && i < whspltd - len; i++) printf(" ");

		printf("%d", p->DEF_LINE_NUM);
		int sizelen = 1;
		int size = p->DEF_LINE_NUM;
		while (size /= 10) sizelen++;
		for (int i = 0; sizelen < whspldr && i < whspldr - sizelen; i++) printf(" ");

		print_aprf_apr_line_nums(p->name);
		printf("\n");

		for (variable_t *pp = p->param; pp != NULL; pp = pp->next) {
			printf("%s:%s", pp->name, p->name);
			for (int i = 0; strlen(pp->name) + strlen(p->name) + 1 < whsplnt && i < whsplnt - strlen(pp->name) - strlen(p->name) - 1; i++) printf(" ");

			int len = print_type_and_get_length(pp->type);
			for (int i = 0; len < whspltd && i < whspltd - len; i++) printf(" ");

			printf("%d", pp->DEF_LINE_NUM);
			sizelen = 1;
			size = pp->DEF_LINE_NUM;
			while (size /= 10) sizelen++;
			for (int i = 0; sizelen < whspldr && i < whspldr - sizelen; i++) printf(" ");

			static char tmp[MAXSTRSIZE];
			sprintf(tmp, "%s:%s", pp->name, p->name);
			print_aprf_apr_line_nums(tmp);
			printf("\n");
		}

		for (variable_t *pv = p->var; pv != NULL; pv = pv->next) {
			printf("%s:%s", pv->name, p->name);
			for (int i = 0; strlen(pv->name) + strlen(p->name) + 1 < whsplnt && i < whsplnt - strlen(pv->name) - strlen(p->name) - 1; i++) printf(" ");

			int len = print_type_and_get_length(pv->type);
			for (int i = 0; len < whspltd && i < whspltd - len; i++) printf(" ");

			printf("%d", pv->DEF_LINE_NUM);
			sizelen = 1;
			size = pv->DEF_LINE_NUM;
			while (size /= 10) sizelen++;
			for (int i = 0; sizelen < whspldr && i < whspldr - sizelen; i++) printf(" ");

			static char tmp[MAXSTRSIZE];
			sprintf(tmp, "%s:%s", pv->name, p->name);
			print_aprf_apr_line_nums(tmp);
			printf("\n");
		}
	}
}
/* ------------------------------------------------------------------------------------ */
