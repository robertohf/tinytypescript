%option yylineno

%{
   #include "tokens.h"

   #define YY_DECL int yylex()

   FILE *fp;
%}

%x CommentBlock

%%
"+" { printf("+"); return '+'; }
"-" { printf("-"); return '-'; }
"*" { printf("*"); return '*'; }
"/" { printf("/"); return '/'; }
"%" { printf("MOD"); return '%'; }
"=" { printf("="); return '='; }
"(" { printf("("); return '('; }
")" { printf(")"); return ')'; }
"{" { printf("{"); return '{'; }
"}" { printf("}"); return '}'; }
"[" { printf("["); return '['; }
"]" { printf("]"); return ']'; }
"^" { printf("^"); return '^'; }
";" { printf(";"); return ';'; }
":" { printf(":"); return ':'; }
"," { printf(","); return ','; }
"." { printf("."); return '.'; }
"<" { printf("<"); return '<'; }
">" { printf(">"); return '>'; }
"!" { printf("!"); return '!'; }
"+=" { printf("+="); return TK_PLUS_EQUAL; }
"&=" { printf("&="); return TK_AND_EQUAL; }
"&&" { printf("&&"); return TK_AND_AND; }
"||" { printf("||"); return TK_OR_OR; }
"==" { printf("=="); return TK_EQUAL_EQUAL; }
"!=" { printf("!="); return TK_NOT_EQUAL; }
"-=" { printf("-="); return TK_MINUS_EQUAL; }
"|=" { printf("|="); return TK_OR_EQUAL; }
"<=" { printf("<="); return TK_LESS_THAN_EQUAL; }
">=" { printf(">="); return TK_GREATER_THAN_EQUAL; }
"*=" { printf("*="); return TK_MULT_EQUAL; }
"/=" { printf("/="); return TK_DIV_EQUAL; }
"%=" { printf("MOD="); return TK_MOD_EQUAL; }
"^=" { printf("^="); return TK_PWR_EQUAL; }
":=" { printf(":="); return TK_ASSIGN; }
"++" { printf("++"); return TK_PLUS_PLUS; }
"--" { printf("--"); return TK_MINUS_MINUS; }
"import" { printf("import"); return KW_IMPORT; }
"export" { printf("export"); return KW_EXPORT; }
"console" { printf("console"); return KW_CONSOLE; }
"log" { printf("log"); return KW_LOG; }
"number" { printf("number"); return KW_NUMBER; }
"string" { printf("string"); return KW_STRING; }
"boolean" { printf("boolean"); return KW_BOOLEAN; }
"Array" { printf("Array"); return KW_ARRAY; }
"break" { printf("break"); return KW_BREAK; }
"const" { printf("const"); return KW_CONST; }
"do" { printf("do"); return KW_DO; }
"else" { printf("else"); return KW_ELSE; }
"for" { printf("for"); return KW_FOR; }
"while" { printf("while"); return KW_WHILE; }
"if" { printf("if"); return KW_IF; }
"in" { printf("in"); return KW_IN; }
"return" { printf("return"); return KW_RETURN; }
"true" { printf("true"); return KW_TRUE; }
"false" { printf("false"); return KW_FALSE; }
"var" { printf("var"); return KW_VAR; }
"let" { printf("let"); return KW_LET; }
"void" { printf("void"); return KW_VOID; }
"of" { printf("of"); return KW_OF; }
"continue" { printf("continue"); return KW_CONTINUE; }
\'.*\' { yylval.string_t = strdup(yytext); printf("%s", yytext); return TK_STRING_LITERAL; }
\".*\" { yylval.string_t = strdup(yytext); printf("%s", yytext); return TK_STRING_LITERAL; }
[a-zA-Z][_|0-9|a-zA-Z]* { yylval.string_t = strdup(yytext); printf("%s", yytext); return TK_IDENTIFIER; }
[0-9]+ { yylval.int_t = atoi(yytext); printf("%s", yytext); return TK_INTEGER; }
[\t] { printf("\t"); }
[\r] { printf("\r"); }
[\n] { printf("\n"); }
[ ] { printf(" "); }
. { printf("\nInvalid Lexical Character %c, Line No. %d", yytext[0], yylineno); return YYUNDEF; }

<CommentBlock>"*/" { BEGIN(INITIAL); }
<CommentBlock>[.|\n] {  }
<CommentBlock><<EOF>> { printf("Block Comment not closed.\n"); }
%%

int yywrap(void) {
   yylineno = 1;
   yyrestart(yyin);
   
   yyin = fp;
   if(yyin == NULL)
      return 0;
   return 1;
}