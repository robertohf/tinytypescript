%option yylineno

%{
   #include "tokens.h"

   #define YY_DECL int yylex()

   FILE *fp;
%}

%x MultiLineComment

%%
"+" { return '+'; }
"-" { return '-'; }
"*" { return '*'; }
"/" { return '/'; }
"%" { return '%'; }
"^" { return '^'; }
"=" { return '='; }
"(" { return '('; }
")" { return ')'; }
"{" { return '{'; }
"}" { return '}'; }
"[" { return '['; }
"]" { return ']'; }
";" { return ';'; }
":" { return ':'; }
"," { return ','; }
"." { return '.'; }
"<" { return '<'; }
">" { return '>'; }
"!" { return '!'; }
"+=" { return TK_PLUS_EQUAL; }
"&=" { return TK_AND_EQUAL; }
"&&" { return TK_AND_AND; }
"||" { return TK_OR_OR; }
"==" { return TK_EQUAL_EQUAL; }
"!=" { return TK_NOT_EQUAL; }
"-=" { return TK_MINUS_EQUAL; }
"|=" { return TK_OR_EQUAL; }
"<=" { return TK_LESS_THAN_EQUAL; }
">=" { return TK_GREATER_THAN_EQUAL; }
"*=" { return TK_MULT_EQUAL; }
"/=" { return TK_DIV_EQUAL; }
"%=" { return TK_MOD_EQUAL; }
"^=" { return TK_PWR_EQUAL; }
":=" { return TK_ASSIGN; }
"++" { return TK_PLUS_PLUS; }
"--" { return TK_MINUS_MINUS; }
"=>" { return TK_ARROW; }
"import" { return KW_IMPORT; }
"console" { return KW_CONSOLE; }
"log" { return KW_LOG; }
"float" { return KW_FLOAT; }
"number" { return KW_NUMBER; }
"int" { return KW_INT; }
"string" { return KW_STRING; }
"boolean" { return KW_BOOLEAN; }
"Array" { return KW_ARRAY; }
"break" { return KW_BREAK; }
"const" { return KW_CONST; }
"do" { return KW_DO; }
"else" { return KW_ELSE; }
"for" { return KW_FOR; }
"while" { return KW_WHILE; }
"if" { return KW_IF; }
"in" { return KW_IN; }
"return" { return KW_RETURN; }
"function" { return KW_FUNCTION; }
"var" { return KW_VAR; }
"let" { return KW_LET; }
"void" { return KW_VOID; }
"of" { return KW_OF; }
"continue" { return KW_CONTINUE; }
"true" { yylval.bool_t = true; return TK_BOOLEAN; }
"false" { yylval.bool_t = false; return TK_BOOLEAN; }
\'.*\' { yylval.string_t = strdup(yytext); return TK_STRING_LITERAL; }
\".*\" { yylval.string_t = strdup(yytext); return TK_STRING_LITERAL; }
[a-zA-Z][_|0-9|a-zA-Z]* { yylval.string_t = strdup(yytext); return TK_IDENTIFIER; }
[0-9]+ { yylval.int_t = atoi(yytext); return TK_INTEGER; }
[0-9]+[\.[0-9]+] { yylval.float_t = atof(yytext); return TK_FLOAT; }
[\t] { }
[\r] { }
[\n] { }
[ ] { }
. { printf("\nInvalid Lexical Character %c, Line No. %d", yytext[0], yylineno); return YYUNDEF; }

"//"[^\n]* { }
"/*" { BEGIN(MultiLineComment); }
<MultiLineComment>"*/" { BEGIN(INITIAL); }
<MultiLineComment>.|\n {  }
<MultiLineComment><<EOF>> { printf("Block Comment not closed.\n"); yyterminate(); }
%%

int yywrap(void) {
   yylineno = 1;
   yyrestart(yyin);
   
   yyin = fp;
   if(yyin == NULL)
      return 0;
   return 1;
}