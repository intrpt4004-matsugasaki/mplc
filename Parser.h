#pragma once

#include "Scanner.h"

extern int token;
extern void update_token();

enum {
	NORMAL	= 0,
	ERROR	= 1,
}
extern int parse_program();
