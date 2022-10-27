#ifndef Tokenounter_h
#define TokenCounter_h

#include "Scanner.h"

typedef struct {
	int CODE;
	char *NAME;
	char *STRING;
	int NUMBER;
} Token_t;

typedef struct {
	Token_t *(* New)(
		const int CODE,
		const char *NAME,
		const char *STRING,
		const int NUMBER
	);
	void (* Delete)(Token_t *);
} _Token;

extern _Token Token;

typedef struct {
	int LENGTH;
	Token_t **TOKEN;

	int _name;
	int _str;
	int _num;
} TokenCounter_t;

typedef struct {
	TokenCounter_t *(* New)();
	void (* AppendToken)(TokenCounter_t *, Token_t *TOKEN);
	void (* PrintTable)(TokenCounter_t *);
	void (* Delete)(TokenCounter_t *);
} _TokenCounter;

extern _TokenCounter TokenCounter;

#endif
