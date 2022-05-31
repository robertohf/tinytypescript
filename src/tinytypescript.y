%{
  #ifdef YYDEBUG 
    yydebug = 1; 
  #endif

  #include <cstdio>
  #include <stdio.h>
  #include <iostream>

  int yylex();
  extern int yyparse();
  extern int yylineno;

  void yyerror(const char * s);
%}

%union {
  const char *string_t;
  int int_t;
  bool bool_t;
};

%token TK_PLUS_EQUAL TK_AND_EQUAL TK_AND_AND TK_OR_OR TK_EQUAL_EQUAL TK_LESS_THAN_EQUAL
%token TK_NOT_EQUAL TK_MINUS_EQUAL TK_OR_EQUAL TK_GREATER_THAN_EQUAL TK_MULT_EQUAL
%token TK_DIV_EQUAL TK_MOD_EQUAL TK_PLUS_PLUS TK_MINUS_MINUS TK_PWR_EQUAL TK_ASSIGN TK_UNDEFINED TK_ARROW
%token KW_NUMBER KW_STRING KW_BOOLEAN KW_ARRAY KW_BREAK KW_CONST KW_DO KW_ELSE KW_IMPORT KW_EXPORT 
%token KW_CONSOLE KW_LOG KW_INT 
%token KW_FOR KW_WHILE KW_IF KW_IN KW_RETURN KW_VAR KW_LET KW_VOID KW_OF KW_CONTINUE KW_FUNCTION
%token<string_t> TK_IDENTIFIER TK_STRING_LITERAL
%token<int_t> TK_INTEGER 
%token<bool_t> TK_BOOLEAN

%left TK_EQUAL_EQUAL TK_NOT_EQUAL
%left '<' '>' TK_LESS_THAN_EQUAL TK_GREATER_THAN_EQUAL
%left '+' '-' 
%left '*' '/' '%' '^'
%left '!'
%left '.'
%left TK_AND_AND TK_OR_OR
%left TK_PLUS_PLUS TK_MINUS_MINUS

%start DeclarationSourceFile

%%
DeclarationSourceFile: BlockStatement
;

TypeAnnotation: KW_VAR
        | KW_LET
        | KW_CONST
        | KW_FUNCTION
;

Type: KW_NUMBER
        | KW_STRING
        | KW_BOOLEAN
        | KW_INT
        | KW_ARRAY
        | KW_VOID
;

BlockStatement: Statement 
        |  BlockStatement Statement 
;

Statement: ImportDeclaration SemiColon
        | LogStatement SemiColon
        | DeclarationStatement SemiColon
        | ReturnStatement SemiColon
        | BreakStatement SemiColon
        | ContinueStatement SemiColon
        | IfStatement
        | LoopStatement
        | Expression SemiColon
;

ImportDeclaration: KW_IMPORT TK_IDENTIFIER '=' TK_IDENTIFIER ';'
;

LogStatement: KW_CONSOLE '.' KW_LOG '(' Expression ')'
;

DeclarationStatement: AssignDeclaration
        | ArrayDeclaration
        | MethodDeclaration 
;

AssignDeclaration: TypeAnnotation TK_IDENTIFIER
        | TypeAnnotation TK_IDENTIFIER ':' Type
        | TypeAnnotation TK_IDENTIFIER '=' Expression
        | TypeAnnotation TK_IDENTIFIER ':' Type '=' Expression
;

AssignOperator: '='
        | TK_ASSIGN
        | TK_AND_EQUAL
        | TK_OR_EQUAL
        | TK_MOD_EQUAL
        | TK_PWR_EQUAL
        | TK_PLUS_EQUAL
        | TK_MINUS_EQUAL
        | TK_MULT_EQUAL
        | TK_DIV_EQUAL
;

MethodDeclaration: TypeAnnotation TK_IDENTIFIER '=' '(' ParameterList ')' TK_ARROW '{' BlockStatement '}'
        | TypeAnnotation TK_IDENTIFIER '=' '(' ParameterList ')' ':' Type TK_ARROW '{' BlockStatement '}'     
        | TypeAnnotation TK_IDENTIFIER '=' '(' ParameterList ')' ':' Type '[' ']' TK_ARROW '{' BlockStatement '}'
        | TypeAnnotation TK_IDENTIFIER '(' ParameterList ')' ':' Type TK_ARROW '{' BlockStatement '}'
        | TypeAnnotation TK_IDENTIFIER '(' ParameterList ')' ':' Type '{' BlockStatement '}'
        | TypeAnnotation TK_IDENTIFIER '(' ParameterList ')' '{' BlockStatement '}'
        | TypeAnnotation TK_IDENTIFIER ':' '(' ParameterList ')' TK_ARROW Type '=' KW_FUNCTION '{' BlockStatement '}'
        | TypeAnnotation TK_IDENTIFIER '=' '(' ')' TK_ARROW '{' BlockStatement '}'
        | TypeAnnotation TK_IDENTIFIER '=' '(' ')' ':' Type TK_ARROW '{' BlockStatement '}'
        | TypeAnnotation TK_IDENTIFIER '=' '(' ')' ':' Type '[' ']' TK_ARROW '{' BlockStatement '}'
        | TypeAnnotation TK_IDENTIFIER '(' ')' ':' Type TK_ARROW '{' BlockStatement '}'
        | TypeAnnotation TK_IDENTIFIER '(' ')' ':' Type '{' BlockStatement '}'
        | TypeAnnotation TK_IDENTIFIER '(' ')' '{' BlockStatement '}'
        | TypeAnnotation TK_IDENTIFIER ':' '(' ')' TK_ARROW Type '=' KW_FUNCTION '{' BlockStatement '}'
;

ParameterList: ParameterList ',' Parameter
        | Parameter
;

Parameter: TK_IDENTIFIER ':' Type
        | TK_IDENTIFIER ':' Type '[' ']'
        | Expression
;

ArrayDeclaration: TypeAnnotation TK_IDENTIFIER '=' ArrayDefinition
        | TypeAnnotation TK_IDENTIFIER ':' Type '[' ']' '=' ArrayDefinition
;

ArrayDefinition: '[' ArgumentExpressionList ']'
        | '[' ']'
;

IfStatement: KW_IF '(' Expression ')' '{' BlockStatement '}'
        | KW_IF '(' Expression KW_IN Expression')' '{' BlockStatement '}'
        | KW_IF '(' Expression ')' '{' BlockStatement '}' KW_ELSE '{' BlockStatement '}'
        | KW_IF '(' Expression ')' '{' BlockStatement '}' ElseIfStatement
        | KW_IF '(' Expression KW_IN Expression')' '{' BlockStatement '}' KW_ELSE '{' BlockStatement '}'
;

ElseIfStatement: KW_ELSE KW_IF '(' Expression ')' '{' BlockStatement '}'
        | ElseIfStatement KW_ELSE KW_IF '(' Expression ')' '{' BlockStatement '}'
;

LoopStatement: KW_WHILE '(' Expression ')' '{' BlockStatement '}'
        | KW_DO '{' BlockStatement '}' KW_WHILE '(' Expression ')' SemiColon
        | KW_FOR '(' LoopExpr ';' LoopExpr ';' LoopExpr ')' '{' BlockStatement '}'
        | KW_FOR '(' TypeAnnotation TK_IDENTIFIER KW_IN TK_IDENTIFIER ')' '{' BlockStatement '}'
;

LoopExpr: %empty
        | Expression
        | DeclarationStatement
;

BreakStatement: KW_BREAK
;

ContinueStatement: KW_CONTINUE
;

ReturnStatement: KW_RETURN Expression
;

Expression: AssignExpression
;

AssignExpression: UnaryExpression AssignOperator Expression
        | LogicalExpression
;

LogicalExpression: LogicalExpression TK_OR_OR EqualityExpression
        | LogicalExpression TK_AND_AND EqualityExpression
        | EqualityExpression
;

EqualityExpression: EqualityExpression TK_EQUAL_EQUAL RelativeExpression
        | EqualityExpression TK_NOT_EQUAL RelativeExpression
        | RelativeExpression
;

RelativeExpression: RelativeExpression '<' ArithmeticExpression
        | RelativeExpression '>' ArithmeticExpression
        | RelativeExpression TK_LESS_THAN_EQUAL ArithmeticExpression
        | RelativeExpression TK_GREATER_THAN_EQUAL ArithmeticExpression
        | ArithmeticExpression
;

ArithmeticExpression: ArithmeticExpression TK_PLUS_PLUS
        | ArithmeticExpression TK_MINUS_MINUS
        | ArithmeticExpression '+' UnaryExpression
        | ArithmeticExpression '-' UnaryExpression
        | ArithmeticExpression '*' UnaryExpression
        | ArithmeticExpression '/' UnaryExpression
        | ArithmeticExpression '%' UnaryExpression
        | ArithmeticExpression '^' UnaryExpression
        | UnaryExpression
;

UnaryExpression: '!' UnaryExpression
        | PostfixExpression
;

PostfixExpression: PostfixExpression '[' ArgumentExpressionList ']'
        | PostfixExpression '(' ')'
        | PostfixExpression '(' ArgumentExpressionList ')'
        | PostfixExpression '.' TK_IDENTIFIER 
        | PrimaryExpression
;

ArgumentExpressionList: ArgumentExpressionList ',' Expression
        | Expression
;

PrimaryExpression: TK_IDENTIFIER
        | TK_INTEGER
        | TK_STRING_LITERAL
        | TK_BOOLEAN
        | '(' Expression ')'
        | ArrayDefinition
;

SemiColon: ';'
;
%%

void yyerror(const char* err) {
  fprintf(stderr, "Line: %d, Error: %s\n", yylineno, err);
  std::cout << "\033[1;31m\n\n--PARSED FAILED--\033[0m" << std::endl; 
  exit(1);
}