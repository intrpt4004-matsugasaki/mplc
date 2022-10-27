#ifndef Parser_h
#define Parser_h

#include "Scanner.h"

static void update_token();

enum {
	NORMAL	= 1,
	ERROR	= 0,
}
extern int parse_program();

#endif
