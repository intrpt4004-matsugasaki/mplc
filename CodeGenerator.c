#include "CodeGenerator.h"

extern char *get_stem(char *filename) {
	char *stem = malloc(sizeof(char) * strlen(filename));
	strcpy(stem, filename);

	int i = 0;
	for (; stem[i] != '\0'; i++);
	for (; stem[i] != '.'; i--);
	for (; stem[i] != '\0'; i++)
		stem[i] = '\0';

	return stem;
}

extern char *assign_csl_ext(char *stem) {
	char *filename = malloc(sizeof(char) * (strlen(stem) + 4));
	sprintf(filename, "%s.csl", stem);
	return filename;
}

static FILE *feedee;
static FEED_STEP step;

static void LABEL(char *label_format, ...) {
	if (step == FEED_COMM) {
		fprintf(feedee, "\n");
	}
	else if (step != FEED_START) {
		printf("[ERROR] CodeGenerator: construction error\n");
		exit(-1);
	}

	va_list ap;
	va_start(ap, label_format);

	vfprintf(feedee, label_format, ap);
	char tmp[MAXSTRSIZE];
	vsprintf(tmp, label_format, ap);
	for (int i = 0; 25 > strlen(tmp) && i < 25 - strlen(tmp); i++) fprintf(feedee, " ");
	fprintf(feedee, " ");

	va_end(ap);

	step = FEED_INST;
}

static void INST(char *opcode, char *operand_format, ...) {
	if (step == FEED_START || step == FEED_COMM) {
		fprintf(feedee, "\n");
		for (int i = 0; i < 25; i++) fprintf(feedee, " ");
		fprintf(feedee, " ");
	}
	else if (step != FEED_INST) {
		printf("[ERROR] CodeGenerator: construction error\n");
		exit(-1);
	}

	fprintf(feedee, "%s", opcode);
	for (int i = 0; 10 > strlen(opcode) && i < 10 - strlen(opcode); i++) fprintf(feedee, " ");
	fprintf(feedee, " ");

	va_list ap;
	va_start(ap, operand_format);

	vfprintf(feedee, operand_format, ap);
	char tmp[MAXSTRSIZE];
	vsprintf(tmp, operand_format, ap);
	for (int i = 0; 15 > strlen(tmp) && i < 15 - strlen(tmp); i++) fprintf(feedee, " ");
	fprintf(feedee, " ");

	va_end(ap);

	step = FEED_COMM;
}

static void FEED() {
	fprintf(feedee, "\n");
	step = FEED_START;
}

static void COMME(char *comment) {
	if (!(step == FEED_START || step == FEED_COMM)) {
		printf("[ERROR] CodeGenerator: construction error\n");
		exit(-1);
	}

	if (step == FEED_START) {
		for (int i = 0; i < 25; i++) fprintf(feedee, " ");
		fprintf(feedee, " ");
	}

	fprintf(feedee, "; %s", comment);
	fprintf(feedee, "\n");

	step = FEED_START;
}

static int Lcnt = 0;

static void generate_code_constant(FILE *feedee, constant_t cons) {
}

static void generate_code_variable_indicator(FILE *feedee, variable_indicator_t var_idr) {
}

static void generate_code_factor(FILE *feedee, factor_t factor) {
	if (factor.kind == VAR_IDR) {
		generate_code_variable_indicator(feedee, factor.var_idr);
	}
	else if (factor.kind == CONST) {
		generate_code_constant(feedee, factor.cons);
	}
	else if (factor.kind == EXPR) {
		generate_code_expression(feedee, *factor.expr);
	}
	else if (factor.kind == INVERT_FACTOR) {
		generate_code_factor(feedee, *factor.inv_factor);
	}
	else if (factor.kind == CAST_EXPR) {
		generate_code_expression(feedee, *factor.cast_expr);
	}
}

static void generate_code_term(FILE *feedee, term_t term) {
	generate_code_factor(feedee, term.factor);

	if (term.next != NULL) {
		generate_code_term(feedee, *term.next);
	}
}


static void generate_code_simple_expression(FILE *feedee, simple_expression_t simp_expr) {
	generate_code_term(feedee, simp_expr.term);

	if (simp_expr.next != NULL) {
		generate_code_simple_expression(feedee, *simp_expr.next);
	}
}

static void generate_code_expression(FILE *feedee, expression_t expr) {
	generate_code_simple_expression(feedee, expr.simp_expr);

	if (expr.next != NULL) {
		generate_code_expression(feedee, *expr.next);
	}
}

static void generate_code_statement(FILE *feedee, statement_t *stmt) {
	if (stmt == NULL) return;

	char tmp[MAXSTRSIZE];

	if (stmt->kind == CALL) {
		call_statement_t *s1 = (call_statement_t *)(stmt);

		for (expressions_t *ep = s1->param; ep != NULL; ep = ep->next) {
			generate_code_expression(feedee, ep->expr);
			INST("PUSH", "GR1");
		}
	
		INST("CALL", "$%s", s1->name);
	}
	else if (stmt->kind == ITERATION) {
		iteration_statement_t *s1 = (iteration_statement_t *)(stmt);

		int l = Lcnt; Lcnt += 2;
		LABEL("#%d", l); INST("NOP", "");
		generate_code_expression(feedee, s1->cond);
		INST("CPL", "GR1, ONE");
		INST("JZ", "#%d", l+1);
		generate_code_statement(feedee, s1->loop_stmt);
		INST("JUMP", "#%d", l);
		LABEL("#%d", l+1); INST("NOP", "");
	}
	else if (stmt->kind == CONDITION) {
		condition_statement_t *s1 = (condition_statement_t *)(stmt);

		int l = Lcnt; Lcnt += 2;
		generate_code_expression(feedee, s1->cond);
		INST("CPL", "GR1, ONE");
		INST("JZ", "#%d", l);
		generate_code_statement(feedee, s1->then_stmt);
		INST("JUMP", "#%d", l+1);
		LABEL("#%d", l); INST("NOP", "");
		if (s1->has_else_stmt) {
			generate_code_statement(feedee, s1->else_stmt);
		}
		LABEL("#%d", l+1); INST("NOP", "");
	}
	else if (stmt->kind == ASSIGN) {
		assignment_statement_t *s1 = (assignment_statement_t *)(stmt);

		generate_code_expression(feedee, s1->expr);
		if (s1->target_var_idr.is_array) {
			//
		}
		INST("ST", "GR1, %%%s", s1->target_var_idr.name);
	}
	else if (stmt->kind == EXIT) {
		// break loop
	}
	else if (stmt->kind == RETURN) {
		INST("RET", "");
	}
	else if (stmt->kind == INPUT) {
		//
	}
	else if (stmt->kind == OUTPUT) {
		//
	}

	generate_code_statement(feedee, stmt->next);
}

static int get_variable_size(variable_t *v) {
	if (v->type.kind == STANDARD) return 1;
	return v->type.array.size;
}

static void outlib();

extern void generate_code(char *filename, program_t program) {
	feedee = fopen(filename, "w");
	step = FEED_START;
	Lcnt = 0;
	char tmp[MAXSTRSIZE];

	LABEL("$%s", get_stem(filename)); INST("START", "$main");
	FEED(); COMME("");

	// program block
	for (variable_t *v = program.var; v != NULL; v = v->next) {
		LABEL("%%%s", v->name); INST("DS", "%d", get_variable_size(v));
	}
	FEED(); COMME("");

	// procedure block
	for (procedure_t *p = program.proc; p != NULL; p = p->next) {
		for (variable_t *v = p->var; v != NULL; v = v->next) {
			LABEL("%%%s.%s", p->name, v->name); INST("DS", "%d", get_variable_size(v));
		}
		if (step != FEED_START) {
			FEED();
			COMME("");
		}

		LABEL("$%s", p->name); INST("DS", "0");

//		for (variable_t *pp = p->param; pp != NULL; pp = pp->next) {
		INST("RPOP", ""); // parameter>8 ???: call stack
//		}

		generate_code_statement(feedee, p->stmt);
		INST("RET", "");
		FEED(); COMME("");
	}

	LABEL("$main"); INST("DS", "0");
	generate_code_statement(feedee, program.stmt);
	INST("END", "");

	outlib();
	fclose(feedee);
}

static void outlib() {
	FEED(); COMME("");
	FEED();
  fprintf(feedee, ""
"; ------------------------\n"
"; Utility functions\n"
"; ------------------------\n"
"EOVF            CALL    WRITELINE\n"
"                LAD     gr1, EOVF1\n"
"                LD      gr2, gr0\n"
"                CALL    WRITESTR\n"
"                CALL    WRITELINE\n"
"                SVC     1  ;  overflow error stop\n"
"EOVF1           DC      '***** Run-Time Error : Overflow *****'\n"
"E0DIV           JNZ     EOVF\n"
"                CALL    WRITELINE\n"
"                LAD     gr1, E0DIV1\n"
"                LD      gr2, gr0\n"
"                CALL    WRITESTR\n"
"                CALL    WRITELINE\n"
"                SVC     2  ;  0-divide error stop\n"
"E0DIV1          DC      '***** Run-Time Error : Zero-Divide *****'\n"
"EROV            CALL    WRITELINE\n"
"                LAD     gr1, EROV1\n"
"                LD      gr2, gr0\n"
"                CALL    WRITESTR\n"
"                CALL    WRITELINE\n"
"                SVC     3  ;  range-over error stop\n"
"EROV1           DC      '***** Run-Time Error : Range-Over in Array Index *****'\n"
"; gr1の値（文字）をgr2のけた数で出力する．\n"
"; gr2が0なら必要最小限の桁数で出力する\n"
"WRITECHAR       RPUSH\n"
"                LD      gr6, SPACE\n"
"                LD      gr7, OBUFSIZE\n"
"WC1             SUBA    gr2, ONE  ; while(--c > 0) {\n"
"                JZE     WC2\n"
"                JMI     WC2\n"
"                ST      gr6, OBUF,gr7  ;  *p++ = ' ';\n"
"                CALL    BOVFCHECK\n"
"                JUMP    WC1  ; }\n"
"WC2             ST      gr1, OBUF,gr7  ; *p++ = gr1;\n"
"                CALL    BOVFCHECK\n"
"                ST      gr7, OBUFSIZE\n"
"                RPOP\n"
"                RET\n"
"; gr1が指す文字列をgr2のけた数で出力する．\n"
"; gr2が0なら必要最小限の桁数で出力する\n"
"WRITESTR        RPUSH\n"
"                LD      gr6, gr1  ; p = gr1;\n"
"WS1             LD      gr4, 0,gr6  ; while(*p != 0) {\n"
"                JZE     WS2\n"
"                ADDA    gr6, ONE  ;  p++;\n"
"                SUBA    gr2, ONE  ;  c--;\n"
"                JUMP    WS1  ; }\n"
"WS2             LD      gr7, OBUFSIZE  ; q = OBUFSIZE;\n"
"                LD      gr5, SPACE\n"
"WS3             SUBA    gr2, ONE  ; while(--c >= 0) {\n"
"                JMI     WS4\n"
"                ST      gr5, OBUF,gr7  ;  *q++ = ' ';\n"
"                CALL    BOVFCHECK\n"
"                JUMP    WS3  ; }\n"
"WS4             LD      gr4, 0,gr1  ; while(*gr1 != 0) {\n"
"                JZE     WS5\n"
"                ST      gr4, OBUF,gr7  ;  *q++ = *gr1++;\n"
"                ADDA    gr1, ONE\n"
"                CALL    BOVFCHECK\n"
"                JUMP    WS4  ; }\n"
"WS5             ST      gr7, OBUFSIZE  ; OBUFSIZE = q;\n"
"                RPOP\n"
"                RET\n"
"BOVFCHECK       ADDA    gr7, ONE\n"
"                CPA     gr7, BOVFLEVEL\n"
"                JMI     BOVF1\n"
"                CALL    WRITELINE\n"
"                LD      gr7, OBUFSIZE\n"
"BOVF1           RET\n"
"BOVFLEVEL       DC      256\n");
fprintf(feedee, 
"; gr1の値（整数）をgr2のけた数で出力する．\n"
"; gr2が0なら必要最小限の桁数で出力する\n"
"WRITEINT        RPUSH\n"
"                LD      gr7, gr0  ; flag = 0;\n"
"                CPA     gr1, gr0  ; if(gr1>=0) goto WI1;\n"
"                JPL     WI1\n"
"                JZE     WI1\n"
"                LD      gr4, gr0  ; gr1= - gr1;\n"
"                SUBA    gr4, gr1\n"
"                CPA     gr4, gr1\n"
"                JZE     WI6\n"
"                LD      gr1, gr4\n"
"                LD      gr7, ONE  ; flag = 1;\n"
"WI1             LD      gr6, SIX  ; p = INTBUF+6;\n"
"                ST      gr0, INTBUF,gr6  ; *p = 0;\n"
"                SUBA    gr6, ONE  ; p--;\n"
"                CPA     gr1, gr0  ; if(gr1 == 0)\n"
"                JNZ     WI2\n"
"                LD      gr4, ZERO  ;  *p = '0';\n"
"                ST      gr4, INTBUF,gr6\n"
"                JUMP    WI5  ; }\n"
"; else {\n"
"WI2             CPA     gr1, gr0  ;  while(gr1 != 0) {\n"
"                JZE     WI3\n"
"                LD      gr5, gr1  ;   gr5 = gr1 - (gr1 / 10) * 10;\n"
"                DIVA    gr1, TEN  ;   gr1 /= 10;\n"
"                LD      gr4, gr1\n"
"                MULA    gr4, TEN\n"
"                SUBA    gr5, gr4\n"
"                ADDA    gr5, ZERO  ;   gr5 += '0';\n"
"                ST      gr5, INTBUF,gr6  ;   *p = gr5;\n"
"                SUBA    gr6, ONE  ;   p--;\n"
"                JUMP    WI2  ;  }\n"
"WI3             CPA     gr7, gr0  ;  if(flag != 0) {\n"
"                JZE     WI4\n"
"                LD      gr4, MINUS  ;   *p = '-';\n"
"                ST      gr4, INTBUF,gr6\n"
"                JUMP    WI5  ;  }\n"
"WI4             ADDA    gr6, ONE  ;  else p++;\n"
"; }\n"
"WI5             LAD     gr1, INTBUF,gr6  ; gr1 = p;\n"
"                CALL    WRITESTR  ; WRITESTR();\n"
"                RPOP\n"
"                RET\n"
"WI6             LAD     gr1, MMINT\n"
"                CALL    WRITESTR  ; WRITESTR();\n"
"                RPOP\n"
"                RET\n"
"MMINT           DC      '-32768'\n"
"; gr1の値（真理値）が0なら'FALSE'を\n"
"; 0以外なら'TRUE'をgr2のけた数で出力する．\n"
"; gr2が0なら必要最小限の桁数で出力する\n"
"WRITEBOOL       RPUSH\n"
"                CPA     gr1, gr0  ; if(gr1 != 0)\n"
"                JZE     WB1\n"
"                LAD     gr1, WBTRUE  ;  gr1 = TRUE;\n"
"                JUMP    WB2\n"
"; else\n"
"WB1             LAD     gr1, WBFALSE  ;  gr1 = FALSE;\n"
"WB2             CALL    WRITESTR  ; WRITESTR();\n"
"                RPOP\n"
"                RET\n"
"WBTRUE          DC      'TRUE'\n"
"WBFALSE         DC      'FALSE'\n"
"; 改行を出力する\n"
"WRITELINE       RPUSH\n"
"                LD      gr7, OBUFSIZE\n"
"                LD      gr6, NEWLINE\n"
"                ST      gr6, OBUF,gr7\n"
"                ADDA    gr7, ONE\n"
"                ST      gr7, OBUFSIZE\n"
"                OUT     OBUF, OBUFSIZE\n"
"                ST      gr0, OBUFSIZE\n"
"                RPOP\n"
"                RET\n"
"FLUSH           RPUSH\n"
"                LD      gr7, OBUFSIZE\n"
"                JZE     FL1\n"
"                CALL    WRITELINE\n"
"FL1             RPOP\n"
"                RET\n"
"; gr1が指す番地に文字一つを読み込む\n"
"READCHAR        RPUSH\n"
"                LD      gr5, RPBBUF  ; if(RPBBUF != 0) {\n"
"                JZE     RC0\n"
"                ST      gr5, 0,gr1  ;  *gr1 = RPBBUF;\n"
"                ST      gr0, RPBBUF  ;  RPBBUF = 0\n"
"                JUMP    RC3  ;  return; }\n"
"RC0             LD      gr7, INP  ; inp = INP;\n"
"                LD      gr6, IBUFSIZE  ; if(IBUFSIZE == 0) {\n"
"                JNZ     RC1\n"
"                IN      IBUF, IBUFSIZE  ;  IN();\n"
"                LD      gr7, gr0  ;  inp = 0;\n"
"; }\n"
"RC1             CPA     gr7, IBUFSIZE  ; if(inp == IBUFSIZE) {\n"
"                JNZ     RC2\n"
"                LD      gr5, NEWLINE  ;  *gr1 = '\\n';\n"
"                ST      gr5, 0,gr1\n"
"                ST      gr0, IBUFSIZE  ;  IBUFSIZE = INP = 0;\n"
"                ST      gr0, INP\n"
"                JUMP    RC3  ; }\n"
"; else {\n"
"RC2             LD      gr5, IBUF,gr7  ;  *gr1 = *inp++;\n"
"                ADDA    gr7, ONE\n"
"                ST      gr5, 0,gr1\n"
"                ST      gr7, INP  ;  INP = inp;\n"
"; }\n"
"RC3             RPOP\n"
"                RET\n");
fprintf(feedee, 
"; gr1が指す番地に整数値一つを読み込む\n"
"READINT         RPUSH\n"
"; do {\n"
"RI1             CALL    READCHAR  ;  ch = READCHAR();\n"
"                LD      gr7, 0,gr1\n"
"                CPA     gr7, SPACE  ; } while(ch==' ' || ch=='\\t' || ch=='\\n');\n"
"                JZE     RI1\n"
"                CPA     gr7, TAB\n"
"                JZE     RI1\n"
"                CPA     gr7, NEWLINE\n"
"                JZE     RI1\n"
"                LD      gr5, ONE  ; flag = 1\n"
"                CPA     gr7, MINUS  ; if(ch == '-') {\n"
"                JNZ     RI4\n"
"                LD      gr5, gr0  ;  flag = 0;\n"
"                CALL    READCHAR  ;  ch = READCHAR();\n"
"                LD      gr7, 0,gr1\n"
"RI4             LD      gr6, gr0  ; v = 0;     ; }\n"
"RI2             CPA     gr7, ZERO  ; while('0' <= ch && ch <= '9') {\n"
"                JMI     RI3\n"
"                CPA     gr7, NINE\n"
"                JPL     RI3\n"
"                MULA    gr6, TEN  ;  v = v*10+ch-'0';\n"
"                ADDA    gr6, gr7\n"
"                SUBA    gr6, ZERO\n"
"                CALL    READCHAR  ;  ch = READSCHAR();\n"
"                LD      gr7, 0,gr1\n"
"                JUMP    RI2  ; }\n"
"RI3             ST      gr7, RPBBUF  ; ReadPushBack();\n"
"                ST      gr6, 0,gr1  ; *gr1 = v;\n"
"                CPA     gr5, gr0  ; if(flag == 0) {\n"
"                JNZ     RI5\n"
"                SUBA    gr5, gr6  ;  *gr1 = -v;\n"
"                ST      gr5, 0,gr1\n"
"; }\n"
"RI5             RPOP\n"
"                RET\n"
"; 入力を改行コードまで（改行コードも含む）読み飛ばす\n"
"READLINE        ST      gr0, IBUFSIZE\n"
"                ST      gr0, INP\n"
"                ST      gr0, RPBBUF\n"
"                RET\n"
"ONE             DC      1\n"
"SIX             DC      6\n"
"TEN             DC      10\n"
"SPACE           DC      #0020  ; ' '\n"
"MINUS           DC      #002D  ; '-'\n"
"TAB             DC      #0009  ; '\\t'\n"
"ZERO            DC      #0030  ; '0'\n"
"NINE            DC      #0039  ; '9'\n"
"NEWLINE         DC      #000A  ; '\\n'\n"
"INTBUF          DS      8\n"
"OBUFSIZE        DC      0\n"
"IBUFSIZE        DC      0\n"
"INP             DC      0\n"
"OBUF            DS      257\n"
"IBUF            DS      257\n"
"RPBBUF          DC      0\n"
  );
}