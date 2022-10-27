#include "Scanner.h"

extern char *tokencode_to_str(const int CODE) {
	switch (CODE) {
		case TNAME:			return "NAME";
		case TPROGRAM:		return "program";
		case TVAR:			return "var";
		case TARRAY:		return "array";
		case TOF:			return "of";
		case TBEGIN:		return "begin";
		case TEND:			return "end";
		case TIF:			return "if";
		case TTHEN:			return "then";
		case TELSE:			return "else";
		case TPROCEDURE:	return "procedure";
		case TRETURN:		return "return";
		case TCALL:			return "call";
		case TWHILE:		return "while";
		case TDO:			return "do";
		case TNOT:			return "not";
		case TOR:			return "or";
		case TDIV:			return "div";
		case TAND:			return "and";
		case TCHAR:			return "char";
		case TINTEGER:		return "integer";
		case TBOOLEAN:		return "boolean";
		case TREADLN:		return "readln";
		case TWRITELN:		return "writeln";
		case TTRUE:			return "true";
		case TFALSE:		return "false";
		case TNUMBER:		return "NUMBER";
		case TSTRING:		return "STRING";
		case TPLUS:			return "+";
		case TMINUS:		return "-";
		case TSTAR:			return "*";
		case TEQUAL:		return "=";
		case TNOTEQ:		return "<>";
		case TLE:			return "<";
		case TLEEQ:			return "<=";
		case TGR:			return ">";
		case TGREQ:			return ">=";
		case TLPAREN:		return "(";
		case TRPAREN:		return ")";
		case TLSQPAREN:		return "[";
		case TRSQPAREN:		return "]";
		case TASSIGN:		return ":=";
		case TDOT:			return ".";
		case TCOMMA:		return ",";
		case TCOLON:		return ":";
		case TSEMI:			return ";";
		case TREAD:			return "read";
		case TWRITE:		return "write";
		case TBREAK:		return "break";
	}
}


static FILE *src;
static int linenum = 1;

int num_attr;
char string_attr[MAXSTRSIZE];
int token_code;

static int c[LOOKAHEADDEPTH];

extern int init_scan(char *filename) {
	src = fopen(filename, "r");
	if (src == NULL) return -1;

	fulfill_char();
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
	} else if (is_alpha()) {
		if (is_keyword()) {
			read_keyword();
			return token_code;
		} else {
			read_name();
			return TNAME;
		}
	} else if (is_symbol()) {
		read_symbol();
		return token_code;
	} else if (is_num()) {
		read_num();
		return TNUMBER;
	} else if (is_string()) {
		read_string();
		return TSTRING;
	} else {
		/* ERROR */
		printf("[ERROR] Scanner: Unrecognized character %c appeared.\n", c[0]);
		exit(-1);
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
	memset(string_attr, '\0', MAXSTRSIZE);
}

static void fulfill_char() {
	for (int i = 0; i < LOOKAHEADDEPTH; i++)
		c[i] = fgetc(src);
}

static void update_char() {
	for (int i = 0; i < LOOKAHEADDEPTH; i++)
		c[i] = c[i+1];

	c[LOOKAHEADDEPTH-1] = fgetc(src);
}

static int is_EOF() {
	return c[0] == EOF;
}

static int is_separator() {
	return c[0] == ' ' || c[0] == '\t'
		|| is_EOL() || is_comment();
}

static int is_EOL() {
	return c[0] == '\r' || c[0] == '\n'
		|| c[0] == '\r' && c[1] == '\n'
		|| c[0] == '\n' && c[1] == '\r';
}

static int is_comment() {
	return c[0] == '{'
		|| c[0] == '/' && c[1] == '*';
}

static void skip_EOL() {
	if (c[1] == '\r' || c[1] == '\n') {
		update_char();
		update_char();
	} else {
		update_char();
	}
}

static void skip_comment() {
	if (is_EOF()) {
		/* ERROR */
		printf("[ERROR] Scanner: EOF reached during comments.\n");
		exit(-1);
	};

	if (c[0] == '}') {
		update_char();
		return;
	}
	
	if (c[0] == '*' && c[1] == '/') {
		update_char();
		update_char();
		return;
	}

	update_char();
	skip_comment();
}

static int is_alpha() {
	return isalpha(c[0]);
}

static int is_keyword_program() {
	return c[0] == 'p' && c[1] == 'r'
		&& c[2] == 'o' && c[3] == 'g'
		&& c[4] == 'r' && c[5] == 'a'
		&& c[6] == 'm' && !isalnum(c[7]);
}

static int is_keyword_var() {
	return c[0] == 'v' && c[1] == 'a'
		&& c[2] == 'r' && !isalnum(c[3]);
}

static int is_keyword_array() {
	return c[0] == 'a' && c[1] == 'r'
		&& c[2] == 'r' && c[3] == 'a'
		&& c[4] == 'y' && !isalnum(c[5]);
}

static int is_keyword_of() {
	return c[0] == 'o' && c[1] == 'f'
		&& !isalnum(c[2]);
}

static int is_keyword_begin() {
	return c[0] == 'b' && c[1] == 'e'
		&& c[2] == 'g' && c[3] == 'i'
		&& c[4] == 'n' && !isalnum(c[5]);
}

static int is_keyword_end() {
	return c[0] == 'e' && c[1] == 'n'
		&& c[2] == 'd' && !isalnum(c[3]);
}

static int is_keyword_if() {
	return c[0] == 'i' && c[1] == 'f'
		&& !isalnum(c[2]);
}

static int is_keyword_then() {
	return c[0] == 't' && c[1] == 'h'
		&& c[2] == 'e' && c[3] == 'n'
		&& !isalnum(c[4]);
}

static int is_keyword_else() {
	return c[0] == 'e' && c[1] == 'l'
		&& c[2] == 's' && c[3] == 'e'
		&& !isalnum(c[4]);
}

static int is_keyword_procedure() {
	return c[0] == 'p' && c[1] == 'r'
		&& c[2] == 'o' && c[3] == 'c'
		&& c[4] == 'e' && c[5] == 'd'
		&& c[6] == 'u' && c[7] == 'r'
		&& c[8] == 'e' && !isalnum(c[9]);
}

static int is_keyword_call() {
	return c[0] == 'c' && c[1] == 'a'
		&& c[2] == 'l' && c[3] == 'l'
		&& !isalnum(c[4]);
}

static int is_keyword_while() {
	return c[0] == 'w' && c[1] == 'h'
		&& c[2] == 'i' && c[3] == 'l'
		&& c[4] == 'e' && !isalnum(c[5]);
}

static int is_keyword_do() {
	return c[0] == 'd' && c[1] == 'o'
		&& !isalnum(c[2]);
}

static int is_keyword_not() {
	return c[0] == 'n' && c[1] == 'o'
		&& c[2] == 't' && !isalnum(c[3]);
}

static int is_keyword_or() {
	return c[0] == 'o' && c[1] == 'r'
		&& !isalnum(c[2]);
}

static int is_keyword_div() {
	return c[0] == 'd' && c[1] == 'i'
		&& c[2] == 'v' && !isalnum(c[3]);
}

static int is_keyword_and() {
	return c[0] == 'a' && c[1] == 'n'
		&& c[2] == 'd' && !isalnum(c[3]);
}

static int is_keyword_char() {
	return c[0] == 'c' && c[1] == 'h'
		&& c[2] == 'a' && c[3] == 'r'
		&& !isalnum(c[4]);
}


static int is_keyword_integer() {
	return c[0] == 'i' && c[1] == 'n'
		&& c[2] == 't' && c[3] == 'e'
		&& c[4] == 'g' && c[5] == 'e'
		&& c[6] == 'r' && !isalnum(c[7]);
}


static int is_keyword_boolean() {
	return c[0] == 'b' && c[1] == 'o'
		&& c[2] == 'o' && c[3] == 'l'
		&& c[4] == 'e' && c[5] == 'a'
		&& c[6] == 'n' && !isalnum(c[7]);
}

static int is_keyword_readln() {
	return c[0] == 'r' && c[1] == 'e'
		&& c[2] == 'a' && c[3] == 'd'
		&& c[4] == 'l' && c[5] == 'n'
		&& !isalnum(c[6]);
}

static int is_keyword_writeln() {
	return c[0] == 'w' && c[1] == 'r'
		&& c[2] == 'i' && c[3] == 't'
		&& c[4] == 'e' && c[5] == 'l'
		&& c[6] == 'n' && !isalnum(c[7]);
}

static int is_keyword_true() {
	return c[0] == 't' && c[1] == 'r'
		&& c[2] == 'u' && c[3] == 'e'
		&& !isalnum(c[4]);
}

static int is_keyword_false() {
	return c[0] == 'f' && c[1] == 'a'
		&& c[2] == 'l' && c[3] == 's'
		&& c[4] == 'e' && !isalnum(c[5]);
}

static int is_keyword_read() {
	return c[0] == 'r' && c[1] == 'e'
		&& c[2] == 'a' && c[3] == 'd'
		&& !isalnum(c[4]);
}

static int is_keyword_write() {
	return c[0] == 'w' && c[1] == 'r'
		&& c[2] == 'i' && c[3] == 't'
		&& c[4] == 'e' && !isalnum(c[5]);
}

static int is_keyword_break() {
	return c[0] == 'b' && c[1] == 'r'
		&& c[2] == 'e' && c[3] == 'a'
		&& c[4] == 'k' && !isalnum(c[5]);
}

static int is_keyword() {
	return is_keyword_program() || is_keyword_var()
		|| is_keyword_array() || is_keyword_of()
		|| is_keyword_begin() || is_keyword_end()
		|| is_keyword_if() || is_keyword_then()
		|| is_keyword_else() || is_keyword_procedure()
		|| is_keyword_call() || is_keyword_while()
		|| is_keyword_do() || is_keyword_not()
		|| is_keyword_or() || is_keyword_div()
		|| is_keyword_and() || is_keyword_char()
		|| is_keyword_integer() || is_keyword_boolean()
		|| is_keyword_readln() || is_keyword_writeln()
		|| is_keyword_true() || is_keyword_false()
		|| is_keyword_read() || is_keyword_write()
		|| is_keyword_break();
}

static void read_keyword() {
	if (is_keyword_program()) {
		for (int i = 0; i < 7; i++) update_char();
		token_code = TPROGRAM;
	}
	
	if (is_keyword_var()) {
		for (int i = 0; i < 3; i++) update_char();
		token_code = TVAR;
	}

	if (is_keyword_array()) {
		for (int i = 0; i < 5; i++) update_char();
		token_code = TARRAY;
	}

	if (is_keyword_of()) {
		for (int i = 0; i < 2; i++) update_char();
		token_code = TOF;
	}

	if (is_keyword_begin()) {
		for (int i = 0; i < 5; i++) update_char();
		token_code = TBEGIN;
	}

	if (is_keyword_end()) {
		for (int i = 0; i < 3; i++) update_char();
		token_code = TEND;
	}

	if (is_keyword_if()) {
		for (int i = 0; i < 2; i++) update_char();
		token_code = TIF;
	}

	if (is_keyword_then()) {
		for (int i = 0; i < 4; i++) update_char();
		token_code = TTHEN;
	}

	if (is_keyword_else()) {
		for (int i = 0; i < 4; i++) update_char();
		token_code = TELSE;
	}

	if (is_keyword_procedure()) {
		for (int i = 0; i < 9; i++) update_char();
		token_code = TPROCEDURE;
	}

	if (is_keyword_call()) {
		for (int i = 0; i < 4; i++) update_char();
		token_code = TCALL;
	}

	if (is_keyword_while()) {
		for (int i = 0; i < 5; i++) update_char();
		token_code = TWHILE;
	}

	if (is_keyword_do()) {
		for (int i = 0; i < 2; i++) update_char();
		token_code = TDO;
	}

	if (is_keyword_not()) {
		for (int i = 0; i < 3; i++) update_char();
		token_code = TNOT;
	}

	if (is_keyword_or()) {
		for (int i = 0; i < 2; i++) update_char();
		token_code = TOR;
	}

	if (is_keyword_div()) {
		for (int i = 0; i < 3; i++) update_char();
		token_code = TDIV;
	}

	if (is_keyword_and()) {
		for (int i = 0; i < 3; i++) update_char();
		token_code = TAND;
	}

	if (is_keyword_char()) {
		for (int i = 0; i < 4; i++) update_char();
		token_code = TCHAR;
	}

	if (is_keyword_integer()) {
		for (int i = 0; i < 7; i++) update_char();
		token_code = TINTEGER;
	}

	if (is_keyword_boolean()) {
		for (int i = 0; i < 7; i++) update_char();
		token_code = TBOOLEAN;
	}

	if (is_keyword_readln()) {
		for (int i = 0; i < 6; i++) update_char();
		token_code = TREADLN;
	}

	if (is_keyword_writeln()) {
		for (int i = 0; i < 7; i++) update_char();
		token_code = TWRITELN;
	}

	if (is_keyword_true()) {
		for (int i = 0; i < 4; i++) update_char();
		token_code = TTRUE;
	}

	if (is_keyword_false()) {
		for (int i = 0; i < 5; i++) update_char();
		token_code = TFALSE;
	}

	if (is_keyword_read()) {
		for (int i = 0; i < 4; i++) update_char();
		token_code = TREAD;
	}

	if (is_keyword_write()) {
		for (int i = 0; i < 5; i++) update_char();
		token_code = TWRITE;
	}

	if (is_keyword_break()) {
		for (int i = 0; i < 5; i++) update_char();
		token_code = TBREAK;
	}
}

static void read_name() {
	for (int i = 0; isalnum(c[0]); i++, update_char()) {
		if (i >= MAXSTRSIZE-1) {
			/* ERROR */
			printf("[ERROR] Scanner: The number of readable characters has been exceeded.\n");
			exit(-1);
		}

		string_attr[i] = c[0];
	}
}

static int is_symbol() {
	return c[0] == '+' || c[0] == '-'
		|| c[0] == '*' || c[0] == '='
		|| c[0] == '<' && c[1] == '>'
		|| c[0] == '<'
		|| c[0] == '<' && c[1] == '='
		|| c[0] == '>'
		|| c[0] == '>' && c[1] == '='
		|| c[0] == '(' || c[0] == ')'
		|| c[0] == '[' || c[0] == ']'
		|| c[0] == ':' && c[1] == '='
		|| c[0] == '.' || c[0] == ','
		|| c[0] == ':' || c[0] == ';';
}

static void read_symbol() {
	switch (c[0]) {
		case '+':
			update_char();
			token_code = TPLUS;
			break;

		case '-':
			update_char();
			token_code = TMINUS;
			break;

		case '*':
			update_char();
			token_code = TSTAR;
			break;

		case '=':
			update_char();
			token_code = TEQUAL;
			break;

		case '<':
			if (c[1] == '>') {
				update_char();
				update_char();
				token_code = TNOTEQ;
			} else if (c[1] == '=') {
				update_char();
				update_char();
				token_code = TLEEQ;
			} else {
				update_char();
				token_code = TLE;
			}
			break;

		case '>':
			if (c[1] == '=') {
				update_char();
				update_char();
				token_code = TGREQ;
			} else {
				update_char();
				token_code = TGR;
			}
			break;

		case '(':
			update_char();
			token_code = TLPAREN;
			break;

		case ')':
			update_char();
			token_code = TRPAREN;
			break;

		case '[':
			update_char();
			token_code = TLSQPAREN;
			break;

		case ']':
			update_char();
			token_code = TRSQPAREN;
			break;

		case ':':
			if (c[1] == '=') {
				update_char();
				update_char();
				token_code = TASSIGN;
			} else {
				update_char();
				token_code = TCOLON;
			}
			break;

		case '.':
			update_char();
			token_code = TDOT;
			break;

		case ',':
			update_char();
			token_code = TCOMMA;
			break;

		case ';':
			update_char();
			token_code = TSEMI;
			break;
	}
}

static int is_num() {
	return isdigit(c[0]);
}

static void read_num() {
	if (!is_num()) return;

	num_attr *= 10;
	num_attr += (int)(c[0]);
	update_char();
	read_num();
}

static int is_string() {
	return c[0] == '\'';
}

static void read_string() {
	update_char();

	for (int i = 0;; i++, update_char()) {
		if (i >= MAXSTRSIZE-2) {
			/* ERROR */
			printf("[ERROR] Scanner: The number of readable characters has been exceeded.\n");
			exit(-1);
		}

		if (c[0] == '\'' && c[1] == '\'') {
			string_attr[i] = c[0];
			string_attr[++i] = c[1];
			update_char();

			continue;
		} else if (c[0] == '\'') {
			update_char();
			return;
		} else
			string_attr[i] = c[0];
	}
}
