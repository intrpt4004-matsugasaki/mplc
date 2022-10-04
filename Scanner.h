#pragma once

#include <stdio.h>
#include <string.h>

#define MAXSTRSIZE 1024

enum {
	TNAME = 1,
	TPROGRAM,
	TVAR, TARRAY, TOF,
	TBEGIN, TEND,
	TIF, TTHEN, TELSE,
	TPROCEDURE, TRETURN, TCALL,
	TWHILE, TDO,
	TNOT, TOR, TDIV, TAND,
	TCHAR, TINTEGER, TBOOLEAN,
	TREADLN, TWRITELN,
	TTRUE, TFALSE,
	TNUMBER, TSTRING,
	TPLUS, TMINUS, TSTAR,
	TEQUAL, TNOTEQ, TLE, TLEEQ, TGR, TGREQ,
	TLPAREN, TRPALEN, TLSQPAREN, TRSQPAREN,
	TASSIGN, TDOT, TCOMMA, TCOLON, TSEMI,
	TREAD, TWRITE, TBREAK
};

//extern const int NUMOFTOKEN;

extern int init_scan(char *filename);
extern int scan();
extern void end_scan();

extern int get_linenum();

extern int num_attr;
extern char string_attr[MAXSTRSIZE];
static void clear_attr();

static void update_char();

static int is_EOF();

static int is_separator();
static int is_EOL();
static int is_comment();
static void skip_EOL();
static void skip_comment();

static int is_num();
static void read_num();
