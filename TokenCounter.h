#pragma once

#include "Scanner.h"

typedef struct {
	int CODE;
	char *STRING;
	int NUMBER;
} Token_t;

typedef struct {
	Token_t *(* New)(const int CODE);
	Token_t *(* NewNum)(const int NUMBER);
	Token_t *(* NewStr)(const char *STRING);
	void (* Delete)(Token_t *);
} _Token;

extern _Token Token;

typedef struct {
	int LENGTH;
	Token_t **TOKEN;
} TokenCounter_t;

typedef struct {
	TokenCounter_t *(* New)();
	void (* AppendToken)(TokenCounter_t *, Token_t *TOKEN);
	void (* PrintTable)(TokenCounter_t *);
	void (* Delete)(TokenCounter_t *);
} _TokenCounter;

extern _TokenCounter TokenCounter;
