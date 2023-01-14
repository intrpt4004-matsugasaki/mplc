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
}

static int is_variables_declared(program_t program, char *var_name) {
	for (variable_t *v = program.var; v != NULL; v = v->next) {
		if (!strcmp(v->name, var_name))
			return 1;
	}
	return 0;
}

static int is_procedures_declared(program_t program, char *proc_name) {
	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		if (!strcmp(p->name, proc_name))
			return 1;
	}
	return 0;
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
