%{
   #ifdef YYDEBUG 
      yydebug = 1; 
   #endif

   #include <cstdio>
   #include <stdio.h>

   using namespace std;

   void yyerror(const char*);
   extern int yyparse();
   extern int yylineno;
   extern int yylex();
%}

%union {
  const char *string_t;
  int int_t;
  bool bool_t;
};

%token TK_PLUS_EQUAL TK_AND_EQUAL TK_AND_AND TK_OR_OR TK_EQUAL_EQUAL TK_LESS_THAN_EQUAL
%token TK_NOT_EQUAL TK_MINUS_EQUAL TK_OR_EQUAL TK_GREATER_THAN_EQUAL TK_MULT_EQUAL
%token TK_DIV_EQUAL TK_MOD_EQUAL TK_PLUS_PLUS TK_MINUS_MINUS TK_PWR_EQUAL TK_ASSIGN TK_UNDEFINED
%token KW_NUMBER KW_STRING KW_BOOLEAN KW_ARRAY KW_BREAK KW_CONST KW_DO KW_ELSE KW_IMPORT KW_EXPORT
%token KW_CONSOLE KW_LOG
%token KW_FOR KW_WHILE KW_IF KW_IN KW_RETURN KW_TRUE KW_FALSE KW_VAR KW_LET KW_VOID KW_OF KW_CONTINUE
%token<string_t> TK_IDENTIFIER TK_STRING_LITERAL
%token<int_t> TK_INTEGER 

%start DeclarationSourceFile

%%
DeclarationSourceFile: DeclarationElementStatement
;

DeclarationElementStatement: /* Empty */
         | DeclarationElement DeclarationElementStatement
;

Type: KW_NUMBER
         | KW_STRING
         | KW_BOOLEAN 
;

DeclarationElement: ImportDeclaration
         | AmbientDeclaration
;

ImportDeclaration: KW_IMPORT TK_IDENTIFIER '=' TK_IDENTIFIER ';'
;

AmbientDeclaration: AmbientVariableDeclaration
;

AmbientVariableDeclaration: KW_VAR TK_IDENTIFIER TypeAnnotation SemiColonOpt
         | KW_CONST TK_IDENTIFIER TypeAnnotation SemiColonOpt
;

TypeAnnotation: /* Empty */
         | ':' Type
;

SemiColonOpt: /* Empty */
         | ';'
;
%%

void yyerror(const char* err) {
  fprintf(stderr, "Line: %d, Error: %s\n", yylineno, err);
  exit(1);
}