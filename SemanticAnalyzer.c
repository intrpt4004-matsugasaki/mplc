#include "SemanticAnalyzer.h"

extern int semantic_analyze() {
	printf("%s\n", prog.NAME);
	for (int i = 0; i < prog.VAR_LEN - 1; i++) {
		printf("%s %s\n", tokencode_to_str(prog.VAR[i].TYPE), prog.VAR[i].NAME);
	}

	return 0;
}
