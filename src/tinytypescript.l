%option yylineno


%{
   #include "tokens.h"

   #define YY_DECL int yylex()

   FILE *fp;
%}

%x CommentBlock

%%
"+" { return '+'; }
"-" { return '-'; }
"*" { return '*'; }
"/" { return '/'; }
"%" { return '%'; }
"=" { return '='; }
"(" { return '('; }
")" { return ')'; }
"{" { return '{'; }
"}" { return '}'; }
"[" { return '['; }
"]" { return ']'; }
"^" { return '^'; }
";" { return ';'; }
":" { return ':'; }
"," { return ','; }
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
"import" { return KW_IMPORT; }
"export" { return KW_EXPORT; }
"number" { return KW_NUMBER; }
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
"true" { return KW_TRUE; }
"false" { return KW_FALSE; }
"var" { return KW_VAR; }
"let" { return KW_LET; }
"void" { return KW_VOID; }
"of" { return KW_OF; }
"continue" { return KW_CONTINUE; }
\'.*\' { yylval.string_t = strdup(yytext); return TK_STRING_LITERAL; }
[a-zA-Z][_|0-9|a-zA-Z]+ { yylval.string_t = strdup(yytext); return TK_IDENTIFIER; }
[0-9]+ { yylval.int_t = atoi(yytext); return TK_INTEGER; }
[\t \r \n] { }
. { printf("Invalid Lexical Character %c, Line No. %d\n", yytext[0], yylineno); }

<CommentBlock>"*/" {BEGIN(INITIAL);}
<CommentBlock>.|\n{  }
<CommentBlock><<EOF>> { printf("Comentario de bloque no cerrado\n");}
%%

int yywrap(void) {
   yyrestart(yyin);
   
   yyin = fp;
   if(yyin == NULL)
      return 0;
   return 1;
}