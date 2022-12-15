#include "SemanticAnalyzer.h"

extern int semantic_analyze() {
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
	}

	return 0;
}
