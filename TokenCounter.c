#include "TokenCounter.h"

static Token_t *Token_New(
	const int CODE,
	const char *NAME,
	const char *STRING,
	const int NUMBER) {

	Token_t *self = malloc(sizeof(Token_t));
	if (self == NULL) {
		/* ERROR */
		printf("[ERROR] TokenCounter: malloc failed.\n");
		exit(-1);
	}
	self->CODE = CODE;

	if (CODE == TNAME) {
		self->NAME = malloc(strlen(NAME));
		if (self->NAME == NULL) {
			/* ERROR */
			printf("[ERROR] TokenCounter: malloc failed.\n");
			exit(-1);
		}
		strcpy(self->NAME, NAME);

	} else if (CODE == TSTRING) {
		self->STRING = malloc(strlen(STRING));
		if (self->STRING == NULL) {
			/* ERROR */
			printf("[ERROR] TokenCounter: malloc failed.\n");
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
		/* ERROR */
		printf("[ERROR] TokenCounter: malloc failed.\n");
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
		/* ERROR */
		printf("[ERROR] TokenCounter: realloc failed.\n");
		exit(-1);
	}
	self->TOKEN[self->LENGTH-1] = TOKEN;

	if (TOKEN->CODE == TNAME) self->_name++;
	else if (TOKEN->CODE == TSTRING) self->_str++;
	else if (TOKEN->CODE == TNUMBER) self->_num++;
}

static void TokenCounter_PrintTable(TokenCounter_t *self) {
	const int MAXTOKENSIZE = TBREAK;
	const int MAXSUPRSIZE = LOOKAHEADDEPTH + LOOKAHEADDEPTH/2;

	int count[MAXTOKENSIZE];
	memset(count, 0, sizeof(count));

	struct {
		char *name;
		int count;
	} name_count[self->_name];
	memset(name_count, 0, sizeof(name_count));
	int nac = 0;

	for (int i = 0; i < self->LENGTH; i++) {
		count[self->TOKEN[i]->CODE]++;

		if (self->TOKEN[i]->CODE == TNAME) {
			int flag = 0;
			for (int j = 0; j < nac; j++) {
				if (!strcmp(self->TOKEN[i]->NAME, name_count[j].name)) {
					name_count[j].count++;
					flag = 1;
				}
			}
			if (!flag) {
				name_count[nac].name = self->TOKEN[i]->NAME;
				name_count[nac++].count = 1;
			}
		}
	}

	for (int i = 0; i < MAXTOKENSIZE; i++) {
		if (count[i] == 0) continue;
		if (i == TNAME) {
			for (int j = 0; j < nac; j++) {
				printf("\"%s", name_count[j].name);
				for (int k = strlen(name_count[j].name); k < MAXSUPRSIZE; k++)
					printf(" ");
				printf("\" %d\n", name_count[j].count);
			}
			continue;
		}

		printf("\"%s", tokencode_to_str(i));
		for (int j = strlen(tokencode_to_str(i)); j < MAXSUPRSIZE; j++)
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
