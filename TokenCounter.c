#include "TokenCounter.h"

static Token_t *Token_New(const int CODE) {
	Token_t *self = malloc(sizeof(Token_t));
	if (self == NULL) {
		printf("ERROR: Token_New\n");
		exit(-1);
	}
	self->CODE = CODE;

	return self;
}

static Token_t *Token_NewStr(const char *STRING) {
	Token_t *self = Token_New(TSTRING);
	self->STRING = malloc(strlen(STRING));
	if (self->STRING == NULL) {
		printf("ERROR: Token_NewStr\n");
		exit(-1);
	}
	strcpy(self->STRING, STRING);

	return self;
}

static Token_t *Token_NewNum(const int NUMBER) {
	Token_t *self = Token_New(TNUMBER);
	self->NUMBER = NUMBER;

	return self;
}

static void Token_Delete(Token_t *self) {
	free(self);
}

_Token Token = {
	.New    = Token_New,
	.NewStr = Token_NewStr,
	.NewNum = Token_NewNum,
	.Delete = Token_Delete
};


static TokenCounter_t *TokenCounter_New() {
	TokenCounter_t *self = malloc(sizeof(Token_t));
	if (self == NULL) {
		printf("ERROR: TokenCounter_New\n");
		exit(-1);
	}
	self->LENGTH	= 0;
	self->TOKEN		= NULL;

	return self;
}

static void TokenCounter_AppendToken(TokenCounter_t *self, Token_t *TOKEN) {
	self->TOKEN = realloc(self->TOKEN, sizeof(Token_t *) * ++self->LENGTH);
	if (self->TOKEN == NULL) {
		printf("ERROR: TokenCounter_AppendToken\n");
		exit(-1);
	}
	self->TOKEN[self->LENGTH-1] = TOKEN;
}

static void TokenCounter_PrintTable(TokenCounter_t *self) {
	union {
		int Int;
		int Num;
		char *Str;
	} count[self->LENGTH];
	memset(count, -1, sizeof(count));

	for (int i = 0; i < self->LENGTH; i++) {
		if (count[self->TOKEN[i]->CODE].Int == -1)
			count[self->TOKEN[i]->CODE].Int = 0;

		count[self->TOKEN[i]->CODE].Int++;
	}

	for (int i = 0; i < self->LENGTH; i++) {
		if (count[i].Int == -1) continue;

		printf("\"%s", tokencode_to_str(i));
		for (int j = strlen(tokencode_to_str(i)); j < LOOKAHEADDEPTH; j++)
			printf(" ");
		printf("\" %d\n", count[i].Int);
	}
}

static void TokenCounter_Delete(TokenCounter_t *self) {
	for (int i = 0; i < self->LENGTH; i++)
		Token.Delete(self->TOKEN[i]);
	free(self->TOKEN);
	free(self);
}

_TokenCounter TokenCounter = {
	.New			= TokenCounter_New,
	.AppendToken	= TokenCounter_AppendToken,
	.PrintTable		= TokenCounter_PrintTable,
	.Delete			= TokenCounter_Delete
};
