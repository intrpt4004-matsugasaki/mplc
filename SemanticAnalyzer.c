#include "SemanticAnalyzer.h"

extern int semantic_analyze() {
	printf("program %s\n", prog.NAME);

	for (int i = 0; i < prog.VAR_LEN; i++) {
		if (prog.VAR[i].TYPE != TARRAY)
			printf("%3d: %s %s\n", i, tokencode_to_str(prog.VAR[i].TYPE), prog.VAR[i].NAME);
		else
			printf("%3d: %s[%s] %s[%d]\n", i,
				tokencode_to_str(prog.VAR[i].TYPE),
				tokencode_to_str(prog.VAR[i].ARR_TYPE),
				prog.VAR[i].NAME,
				prog.VAR[i].ARR_NUM
			);
	}

	for (int i = 0; i < prog.PROC_LEN; i++) {
		printf("%3d: procedure %s(", i, prog.PROC[i].NAME);

		for (int j = 0; j < prog.PROC[i].PARAM_LEN; j++) {
			if (j != 0) printf(", ");
			printf("%s %s",
				tokencode_to_str(prog.PROC[i].PARAM[j].TYPE),
				prog.PROC[i].PARAM[j].NAME
			);
		}

		printf(")\n");
	}

	return 0;
}
