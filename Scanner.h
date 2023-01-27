#ifndef Scanner_h
#define Scanner_h

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAXSTRSIZE 1024
#define LOOKAHEADDEPTH 10

typedef enum {
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
	TLPAREN, TRPAREN, TLSQPAREN, TRSQPAREN,
	TASSIGN, TDOT, TCOMMA, TCOLON, TSEMI,
	TREAD, TWRITE, TBREAK
} token_code_t;

extern int init_scan(char *filename);
extern int scan();
extern void end_scan();

extern int get_linenum();

extern int num_attr;
extern char string_attr[MAXSTRSIZE];
static void clear_attr();

static void fulfill_char();
static void update_char();

static int is_EOF();

static int is_separator();
static int is_EOL();
static int is_comment();
static void skip_EOL();
static void skip_comment();

static int is_alpha();
static int is_keyword();
static void read_keyword();
static void read_name();

static int is_symbol();
static void read_symbol();

static int is_num();
static void read_num();

static int is_string();
static void read_string();

#endif
