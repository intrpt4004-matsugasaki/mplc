#ifndef CodeGenerator_h
#define CodeGenerator_h

#include "Parser.h"

extern char *get_stem(char *filename);
extern void generate_code(char *filename, program_t program);

#endif
