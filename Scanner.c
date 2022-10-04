#include "Scanner.h"

static FILE *src;
static int linenum = 1;

int num_attr;
char string_attr[MAXSTRSIZE];

static int c0, c1;

//const int NUMOFTOKEN = TBREAK;

extern int init_scan(char *filename) {
	src = fopen(filename, "r");
	if (src == NULL) return -1;

	c1 = fgetc(src);
	update_char();

	return 0;
}

extern int scan() {
	clear_attr();

	top:
	if (is_EOF()) {
		return -1;
	} else if (is_separator()) {
		if (is_EOL())
			skip_EOL();
		else if (is_comment())
			skip_comment();
		else
			update_char();
		goto top;
	} else if (is_num()) {
		read_num();
		return TNUMBER;
	}
}

extern void end_scan() {
	fclose(src);
}

extern int get_linenum() {
	return linenum;
}

static void clear_attr() {
	num_attr = 0;
	memset(string_attr, 0, strlen(string_attr));
}

static void update_char() {
	c0 = c1;
	c1 = fgetc(src);
}

static int is_EOF() {
	return feof(src);
}

static int is_separator() {
	return c0 == ' ' || c0 == '\t'
		|| is_EOL() || is_comment();
}

static int is_EOL() {
	return c0 == '\r' || c0 == '\n'
		|| c0 == '\r' && c1 == '\n'
		|| c0 == '\n' && c1 == '\r';
}

static int is_comment() {
	return c0 == '{'
		|| c0 == '/' && c1 == '*';
}

static void skip_EOL() {
	if (c1 == '\r' || c1 == '\n') {
		update_char();
		update_char();
	} else {
		update_char();
	}
}

static void skip_comment() {
	if (is_EOF()) return;

	if (c0 == '}') return;
	if (c0 == '*' && c1 == '/') {
		update_char();
		return;
	}

	update_char();
	skip_comment();
}

static int is_num() {
	return c0 == '0' || c0 == '1' || c0 == '2'
		|| c0 == '3' || c0 == '4' || c0 == '5'
		|| c0 == '6' || c0 == '7' || c0 == '8'
		|| c0 == '9';
}

static void read_num() {
	if (!is_num()) return;

	num_attr *= 10;
	num_attr += (int)(c0);
	update_char();
	read_num();
}
