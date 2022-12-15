#include "SemanticAnalyzer.h"

extern int semantic_analyze() {
	_dump();

	return 0;
}

static void print_xref_table() {
}

static void _dump() {
	printf("program %s\n", program.NAME);

	for (int i = 0; i < program.VAR_LEN; i++) {
		if (program.VAR[i].TYPE != TARRAY)
			printf("%3d: %s %s\n", i, tokencode_to_str(program.VAR[i].TYPE), program.VAR[i].NAME);
		else
			printf("%3d: %s[%s] %s[%d]\n", i,
				tokencode_to_str(program.VAR[i].TYPE),
				tokencode_to_str(program.VAR[i].ARR_TYPE),
				program.VAR[i].NAME,
				program.VAR[i].ARR_NUM
			);
	}

	for (int j = 0; j < program.STMT_LEN; j++)
		printf("   %3d: %s\n", j, statementcode_to_str(program.STMT[j].KIND));

	for (int i = 0; i < program.PROC_LEN; i++) {
		printf("%3d: procedure %s(", i, program.PROC[i].NAME);

		for (int j = 0; j < program.PROC[i].PARAM_LEN; j++) {
			if (j != 0) printf(", ");
			printf("%s %s",
				tokencode_to_str(program.PROC[i].PARAM[j].TYPE),
				program.PROC[i].PARAM[j].NAME
			);
		}
		printf(")\n");

		for (int j = 0; j < program.PROC[i].VAR_LEN; j++) {
			printf("   %3d: %s %s\n",
				j,
				tokencode_to_str(program.PROC[i].VAR[j].TYPE),
				program.PROC[i].VAR[j].NAME
			);
		}

		for (int j = 0; j < program.PROC[i].STMT_LEN; j++)
			printf("   %3d: %s\n", j, statementcode_to_str(program.PROC[i].STMT[j].KIND));
	}
}