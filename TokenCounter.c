#include "TokenCounter.h"

static Token_t *Token_New(
	const int CODE,
	const char *NAME,
	const char *STRING,
	const int NUMBER) {

	Token_t *self = malloc(sizeof(Token_t));
	if (self == NULL) {
		printf("ERROR: Token_New\n");
		exit(-1);
	}
	self->CODE = CODE;

	if (CODE == TNAME) {
		self->NAME = malloc(strlen(NAME));
		if (self->NAME == NULL) {
			printf("ERROR: Token_New\n");
			exit(-1);
		}
		strcpy(self->NAME, NAME);

	} else if (CODE == TSTRING) {
		self->STRING = malloc(strlen(STRING));
		if (self->STRING == NULL) {
			printf("ERROR: Token_New\n");
			exit(-1);
		}
		strcpy(self->STRING, STRING);

	} else if (CODE == TNUMBER)
		self->NUMBER = NUMBER;

	return self;
}

static void Token_Delete(Token_t *self) {
	free(self);
}

_Token Token = {
	.New		= Token_New,
	.Delete		= Token_Delete
};


static TokenCounter_t *TokenCounter_New() {
	TokenCounter_t *self = malloc(sizeof(Token_t));
	if (self == NULL) {
		printf("ERROR: TokenCounter_New\n");
		exit(-1);
	}
	self->LENGTH	= 0;
	self->TOKEN		= NULL;

	self->_name		= 0;
	self->_str		= 0;
	self->_num		= 0;

	return self;
}

static void TokenCounter_AppendToken(TokenCounter_t *self, Token_t *TOKEN) {
	self->TOKEN = realloc(self->TOKEN, sizeof(Token_t *) * ++self->LENGTH);
	if (self->TOKEN == NULL) {
		printf("ERROR: TokenCounter_AppendToken\n");
		exit(-1);
	}
	self->TOKEN[self->LENGTH-1] = TOKEN;

	if (TOKEN->CODE == TNAME) self->_name++;
	else if (TOKEN->CODE == TSTRING) self->_str++;
	else if (TOKEN->CODE == TNUMBER) self->_num++;
}

static void TokenCounter_PrintTable(TokenCounter_t *self) {
	const int MAXTOKENSIZE = TBREAK;

	int count[MAXTOKENSIZE];
	memset(count, 0, sizeof(count));

	for (int i = 0; i < self->LENGTH; i++)
		count[self->TOKEN[i]->CODE]++;

	for (int i = 0; i < MAXTOKENSIZE; i++) {
		if (count[i] == 0) continue;
		if (i == TNAME) {
			//
			continue;
		} else if (i == TSTRING) {
			//
			continue;
		} else if (i == TNUMBER) {
			continue;
		}

		printf("\"%s", tokencode_to_str(i));
		for (int j = strlen(tokencode_to_str(i)); j < LOOKAHEADDEPTH; j++)
			printf(" ");
		printf("\" %d\n", count[i]);
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
