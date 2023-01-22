#include "CodeGenerator.h"

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

static int get_size(variable_t *v) {
	if (v->type.kind == STANDARD) return 1;
	return v->type.array.size;
}

extern void generate_code(char *filename, program_t program) {
	FILE *fp = fopen(filename, "w");

	// program block
	for (variable_t *v = program.var; v != NULL; v = v->next) {
		fprintf(fp, "$%s     DS     %d\n", v->name, get_size(v));
	}

	// procedure block
	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		for (variable_t *v = p->var; v != NULL; v = v->next) {
			fprintf(fp, "$%s.%s     DS     %d\n", p->name, v->name, get_size(v));
		}
	}

	fprintf(fp, "END\n");
	fclose(fp);
}
