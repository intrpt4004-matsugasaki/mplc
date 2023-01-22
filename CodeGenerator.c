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

extern void generate_code(char *filename, program_t program) {
	printf("%s", filename);
}
