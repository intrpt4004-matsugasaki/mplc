#include "Parser.h"

static int error(char *message) {
	printf("ERROR: %s\n", message);
	end_scan();

	return ERROR;
}

extern void update_token() {
	token = scan();
}

static int check(int TOKEN_CODE) {
	update_token();
	return token == TOKEN_CODE;
}

extern int parse_program() {
	if (!check(TPROGRAM))
		return error("Keyword 'program' is not found");

	if (!check(TNAME))
		return error("");

	if (!check(TSEMI))
		return error("");

	if (!parse_block())
		return error("");

	if (!check(TDOT))
		return error("");

	return NORMAL;
}

static int parse_block() {
	parse_vardecl();
	return NORMAL;
}
