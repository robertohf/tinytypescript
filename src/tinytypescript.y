%code requires{
    #include "../src/ast.h"
}

%{
    #ifdef YYDEBUG 
        yydebug = 1; 
    #endif
    
    #include <cstdio>
    #include <stdio.h>
    #include <iostream>
    #include <fstream>
    #include "../src/ast.h"
    
    #define EQUAL 1
    #define PLUSEQUAL 2
    #define MINUSEQUAL 3
    #define MULTEQUAL 4
    #define DIVEQUAL 5
    #define ASSIGN 6
    #define ANDEQUAL 7
    #define OREQUAL 8
    #define MODEQUAL 9
    #define PWREQUAL 10

    int yylex();
    extern int yylineno;
    
    Asm assemblyFile;

    void write(std::string name);
    void yyerror(const char * s);
%}

%union {
    const char *string_t;
    int int_t;
    float float_t;
    bool bool_t;

    Stmt *stmt_t;
    Expr *expr_t;
    Parameter *parameter_t;
    Declaration *declaration_t;
    Declarator *declarator_t;
    StmtList *stmt_list_t;
    ParameterList *parameter_list_t;
    ArgumentList *argument_list_t;
    InitializerList *initializer_t;
    InitDeclaration *init_declaration_t;
    InitDeclarationList *init_declaration_list_t;
};

%token TK_PLUS_EQUAL TK_AND_EQUAL TK_AND_AND TK_OR_OR TK_EQUAL_EQUAL TK_LESS_THAN_EQUAL
%token TK_NOT_EQUAL TK_MINUS_EQUAL TK_OR_EQUAL TK_GREATER_THAN_EQUAL TK_MULT_EQUAL
%token TK_DIV_EQUAL TK_MOD_EQUAL TK_PLUS_PLUS TK_MINUS_MINUS TK_PWR_EQUAL TK_ASSIGN TK_ARROW
%token KW_NUMBER KW_FLOAT KW_STRING KW_BOOLEAN KW_ARRAY KW_BREAK KW_CONST KW_DO KW_ELSE KW_IMPORT
%token KW_CONSOLE KW_LOG KW_INT
%token KW_FOR KW_WHILE KW_IF KW_IN KW_RETURN KW_VAR KW_LET KW_VOID KW_OF KW_CONTINUE KW_FUNCTION
%token<string_t> TK_IDENTIFIER TK_STRING_LITERAL
%token<int_t> TK_INTEGER 
%token<float_t> TK_FLOAT
%token<bool_t> TK_BOOLEAN

%left TK_EQUAL_EQUAL TK_NOT_EQUAL
%left '<' '>' TK_LESS_THAN_EQUAL TK_GREATER_THAN_EQUAL
%left '+' '-'  
%left '*' '/' '%' '^'
%left '!'
%left '.'
%left TK_AND_AND TK_OR_OR
%left TK_PLUS_PLUS TK_MINUS_MINUS

%type<declarator_t> Declarator
%type<declaration_t> VariableDeclaration
%type<initializer_t> Initializer
%type<parameter_t> Parameter
%type<parameter_list_t> ParameterList 
%type<argument_list_t> ArgumentExpressionList
%type<int_t> Type AssignOperator
%type<stmt_t> Statement ImportDeclaration LogStatement DeclarationStatement ReturnStatement BreakStatement
%type<stmt_t> ContinueStatement IfStatement LoopStatement ExpressionStmt MethodDeclaration
%type<expr_t> Expression AssignExpression LogicalExpression EqualityExpression RelativeExpression ArithmeticExpression
%type<expr_t> UnaryExpression PostfixExpression PrimaryExpression
%type<stmt_list_t> BlockStatement
%type<init_declaration_list_t> InitDeclarationList
%type<init_declaration_t> InitDeclaration

%start DeclarationSourceFile

%%
DeclarationSourceFile: BlockStatement {
                                        assemblyFile.global = ".globl main\n";
                                        assemblyFile.data = ".data\n";
                                        assemblyFile.text = ".text\nmain:\n";

                                        Stmt *root = new BlockStmt($1);
                                        std::string code = root->generateCode();

                                        assemblyFile.text += code;
                                        write("result.asm");
                                    }
;

BlockStatement: Statement { $$ = new StmtList; $$->push_back($1); }
        |  BlockStatement Statement { $$ = $1; $$->push_back($2); }
;

Statement: ImportDeclaration SemiColon { $$ = $1; }
        | LogStatement SemiColon { $$ = $1; }
        | DeclarationStatement SemiColon { $$ = $1; }
        | ReturnStatement SemiColon { $$ = $1; }
        | BreakStatement SemiColon { $$ = $1; }
        | ContinueStatement SemiColon { $$ = $1; }
        | IfStatement { $$ = $1; }
        | LoopStatement { $$ = $1; }
        | ExpressionStmt SemiColon { $$ = $1; }
;

ImportDeclaration: KW_IMPORT TK_IDENTIFIER '=' TK_IDENTIFIER ';' {}
;

LogStatement: KW_CONSOLE '.' KW_LOG '(' Expression ')' { $$ = new PrintStmt($5); }
;

DeclarationStatement: VariableDeclaration { $$ = $1; }
        /* | ArrayDeclaration { $$ = $1; } */
        | MethodDeclaration { $$ = $1; }
;

TypeAnnotation: KW_VAR
        | KW_LET
        | KW_CONST
        | KW_FUNCTION
;

Type: KW_FLOAT { $$ = FLOAT; }
        | KW_STRING { $$ = STRING; }
        | KW_BOOLEAN { $$ = BOOL; }
        | KW_INT { $$ = INT; }
        | KW_NUMBER { $$ = INT; }
        | KW_VOID { $$ = VOID; }
;

VariableDeclaration: TypeAnnotation InitDeclarationList ':' Type { $$ = new Declaration((Type)$4, $2); }
;

InitDeclarationList: InitDeclaration { $$ = new InitDeclarationList; $$->push_back($1); }
        | InitDeclarationList ',' InitDeclaration { $$ = $1; $$->push_back($3); }
;

InitDeclaration: Declarator { $$ = new InitDeclaration($1, (new InitializerList)); }
        | Declarator '=' Initializer { $$ = new InitDeclaration($1, $3); }
;

Declarator: TK_IDENTIFIER { $$ = new Declarator($1, NULL, false); }
        | TK_IDENTIFIER '[' Expression ']' { $$ = new Declarator($1, $3, true); }
;

Initializer: Expression { $$ = new InitializerList; $$->push_back($1); }
;

/* VariableDeclaration: TypeAnnotation TK_IDENTIFIER ':' Type { $$ = new AssignStmt((IdentExpr *)$2, nullptr); }
        | TypeAnnotation TK_IDENTIFIER ':' Type '=' Expression { $$ = new AssignStmt((IdentExpr *)$2, $6); }
; */

/* ArrayDeclaration: TypeAnnotation TK_IDENTIFIER '=' ArrayDefinition {}
        | TypeAnnotation TK_IDENTIFIER ':' Type '[' ']' '=' ArrayDefinition {}
;

ArrayDefinition: '[' ArgumentExpressionList ']' {}
        | '[' ']' {}
; */

MethodDeclaration: TypeAnnotation TK_IDENTIFIER '=' '(' ParameterList ')' ':' Type TK_ARROW '{' BlockStatement '}' { $$ = new MethodDeclaration($2, $5, (Type)$8, $11); }
        /* | TypeAnnotation TK_IDENTIFIER '=' '(' ParameterList ')' ':' Type '[' ']' TK_ARROW '{' BlockStatement '}' { $$ = new MethodDeclaration($2, $5, $8, ); } */
        | TypeAnnotation TK_IDENTIFIER '(' ParameterList ')' ':' Type TK_ARROW '{' BlockStatement '}' { $$ = new MethodDeclaration($2, $4, (Type)$7, $10); }
        | TypeAnnotation TK_IDENTIFIER '(' ParameterList ')' ':' Type '{' BlockStatement '}' { $$ = new MethodDeclaration($2, $4, (Type)$7, $9); }
        | TypeAnnotation TK_IDENTIFIER ':' '(' ParameterList ')' TK_ARROW Type '=' KW_FUNCTION '{' BlockStatement '}' { $$ = new MethodDeclaration($2, $5, (Type)$8, $12); }
        | TypeAnnotation TK_IDENTIFIER '=' '(' ')' ':' Type TK_ARROW '{' BlockStatement '}' { $$ = new MethodDeclaration($2, nullptr, (Type)$7, $10); }
        /* | TypeAnnotation TK_IDENTIFIER '=' '(' ')' ':' Type '[' ']' TK_ARROW '{' BlockStatement '}' { $$ = new MethodDeclaration($2); } */
        | TypeAnnotation TK_IDENTIFIER '(' ')' ':' Type TK_ARROW '{' BlockStatement '}' { $$ = new MethodDeclaration($2, nullptr, (Type)$6, $9); }
        | TypeAnnotation TK_IDENTIFIER '(' ')' ':' Type '{' BlockStatement '}' { $$ = new MethodDeclaration($2, nullptr, (Type)$6, $8); }
        | TypeAnnotation TK_IDENTIFIER ':' '(' ')' TK_ARROW Type '=' KW_FUNCTION '{' BlockStatement '}' { $$ = new MethodDeclaration($2, nullptr, (Type)$7, $11); }
;

ParameterList: Parameter { $$ = new ParameterList; $$->push_back($1); }
        | ParameterList ',' Parameter { $$ = $1; $$->push_back($3); }
;

Parameter: TK_IDENTIFIER ':' Type { $$ = new Parameter((Type)$3, new Declarator($1, nullptr, false)); }
        | TK_IDENTIFIER ':' Type '[' Expression ']' { $$ = new Parameter((Type)$3, new Declarator($1, $5, true)); }
        /* | TK_IDENTIFIER ':' Type '[' ']' { $$ = new Parameter($3, new Declarator($1, (NumExpr *)50, true)); } */
;

IfStatement: KW_IF '(' Expression ')' '{' BlockStatement '}' { $$ = new IfStmt($3, $6, nullptr); }
        /* | KW_IF '(' Expression KW_IN Expression')' '{' BlockStatement '}' {} */
        | KW_IF '(' Expression ')' '{' BlockStatement '}' KW_ELSE '{' BlockStatement '}' { $$ = new IfStmt($3, $6, $10); }
        /* | KW_IF '(' Expression ')' '{' BlockStatement '}' ElseIfStatement {} */
        /* | KW_IF '(' Expression KW_IN Expression')' '{' BlockStatement '}' KW_ELSE '{' BlockStatement '}' {} */
;

/* ElseIfStatement: KW_ELSE KW_IF '(' Expression ')' '{' BlockStatement '}' {}
        | ElseIfStatement KW_ELSE KW_IF '(' Expression ')' '{' BlockStatement '}' {}
; */

LoopStatement: KW_WHILE '(' Expression ')' '{' BlockStatement '}' { $$ = new WhileStmt($3, $6); }
        /* | KW_DO '{' BlockStatement '}' KW_WHILE '(' Expression ')' SemiColon {} */
        | KW_FOR '(' VariableDeclaration ';' Expression ';' AssignExpression ')' '{' BlockStatement '}' { $$ = new ForStmt($3, $5, $7, $10); }
        /* | KW_FOR '(' TypeAnnotation TK_IDENTIFIER KW_IN TK_IDENTIFIER ')' '{' BlockStatement '}' {} */
;

ExpressionStmt: Expression { $$ = new ExprStmt($1); }
;

BreakStatement: KW_BREAK { $$ = new BreakStmt(); }
;

ContinueStatement: KW_CONTINUE { $$ = new ContinueStmt(); }
;

ReturnStatement: KW_RETURN Expression { $$ = new ReturnStmt($2); }
;

Expression: AssignExpression { $$ = $1; }
;

AssignExpression: PostfixExpression AssignOperator Expression { $$ = new AssignExpr($1, $3); }
        | LogicalExpression { $$ = $1; }
;

AssignOperator: '=' { $$ = EQUAL; }
        | TK_ASSIGN { $$ = ASSIGN; }
        | TK_AND_EQUAL { $$ = ANDEQUAL; }
        | TK_OR_EQUAL { $$ = OREQUAL; }
        | TK_MOD_EQUAL { $$ = MODEQUAL; }
        | TK_PWR_EQUAL { $$ = PWREQUAL; }
        | TK_PLUS_EQUAL { $$ = PLUSEQUAL; }
        | TK_MINUS_EQUAL { $$ = MINUSEQUAL; }
        | TK_MULT_EQUAL { $$ = MULTEQUAL; }
        | TK_DIV_EQUAL { $$ = DIVEQUAL; }
;

LogicalExpression: LogicalExpression TK_OR_OR EqualityExpression { $$ = new OrExpr($1, $3); }
        | LogicalExpression TK_AND_AND EqualityExpression { $$ = new AndExpr($1, $3); }
        | EqualityExpression { $$ = $1; }
;

EqualityExpression: EqualityExpression TK_EQUAL_EQUAL RelativeExpression { $$ = new EqualsExpr($1, $3); }
        | EqualityExpression TK_NOT_EQUAL RelativeExpression { $$ = new NotEqualExpr($1, $3); }
        | RelativeExpression { $$ = $1; }
;

RelativeExpression: RelativeExpression '<' ArithmeticExpression { $$ = new LtExpr($1, $3); }
        | RelativeExpression '>' ArithmeticExpression { $$ = new GtExpr($1, $3); }
        | RelativeExpression TK_LESS_THAN_EQUAL ArithmeticExpression { $$ = new LteExpr($1, $3); }  
        | RelativeExpression TK_GREATER_THAN_EQUAL ArithmeticExpression { $$ = new GteExpr($1, $3); }
        | ArithmeticExpression { $$ = $1; }
;

ArithmeticExpression: ArithmeticExpression TK_PLUS_PLUS { $$ = new AddExpr($1, (IntExpr *)1); }
        | ArithmeticExpression TK_MINUS_MINUS { $$ = new SubExpr($1, (IntExpr *)1); }
        | ArithmeticExpression '+' UnaryExpression { $$ = new AddExpr($1, $3); }
        | ArithmeticExpression '-' UnaryExpression { $$ = new SubExpr($1, $3); }
        | ArithmeticExpression '*' UnaryExpression { $$ = new MultExpr($1, $3); }
        | ArithmeticExpression '/' UnaryExpression { $$ = new DivExpr($1, $3); }
        | ArithmeticExpression '%' UnaryExpression { $$ = new ModExpr($1, $3); }
        | ArithmeticExpression '^' UnaryExpression { $$ = new PwrExpr($1, $3); }
        | UnaryExpression { $$ = $1; }
;

UnaryExpression: '!' UnaryExpression { $$ = new NotExpr($2); }
        | PostfixExpression { $$ = $1; }
;

PostfixExpression: PostfixExpression '[' ArgumentExpressionList ']' { $$ = new ArrayExpr((IdentExpr *)$1, $3); }
        | PostfixExpression '(' ')' { $$ = new MethodInvocation((IdentExpr *)$1, new ArgumentList); }
        | PostfixExpression '(' ArgumentExpressionList ')' { $$ = new MethodInvocation((IdentExpr *)$1, $3); }
        | PrimaryExpression { $$ = $1; }
;

ArgumentExpressionList: Expression { $$ = new ArgumentList; $$->push_back($1); }
        |  ArgumentExpressionList ',' Expression { $$ = $1; $$->push_back($3); }
;

PrimaryExpression: TK_IDENTIFIER { $$ = new IdentExpr($1); }
        | TK_INTEGER { $$ = new IntExpr($1); }
        | TK_FLOAT { $$ = new FloatExpr($1); }
        | TK_STRING_LITERAL { $$ = new StringExpr($1); }
        | TK_BOOLEAN { $$ = new BoolExpr($1); }
        | '(' Expression ')' { $$ = $2; }
;

SemiColon: ';'
;
%%

void write(std::string name) {
    std::ofstream file;
    file.open(name);

    file << assemblyFile.data << std::endl
    << assemblyFile.global << std::endl 
    << assemblyFile.text << std::endl;

    file << "li $v0, 10" << std::endl
    << "syscall"  << std::endl;
    file.close();
}

void yyerror(const char* err) {
    fprintf(stderr, "Line: %d, Error: %s\n", yylineno, err);
    std::cout << "\033[1;31m\n\n--PARSED FAILED--\033[0m" << std::endl; 
    exit(1);
}