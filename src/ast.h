#ifndef _AST_H
#define _AST_H

#include <string>
#include <sstream>
#include <unordered_map>
#include <map>
#include <list>
#include <iostream>
#include "code.h"
#include "asm.h"

class Expr;
class Stmt;
class Parameter;
class Declaration;
class InitDeclaration;

typedef Expr *ExprPtr;
typedef Stmt *StmtPtr;

typedef std::list<InitDeclaration *> InitDeclarationList;
typedef std::list<Expr *> InitializerList;
typedef std::list<Expr *> ArgumentList;
typedef std::list<Parameter *> ParameterList;
typedef std::list<Declaration *> DeclarationList;
typedef std::list<Stmt *> StmtList;

/**
 * @brief AstNode (Root)
 * 
**/
class AstNode {
public:
    AstNode() {}
    virtual ~AstNode() {}
};

/**
 * @brief Expressions (Each Expr Should Generate Its Own Code)
 * 
**/
class Expr : public AstNode {
public:
    Expr() {}
    virtual ~Expr() {}
    int line;
    virtual Type evalType() = 0;
    virtual void codeGenerator(Code &context) = 0;
};

class AssignExpr : public Expr {
public:
    AssignExpr(ExprPtr id, ExprPtr rightExpr) 
    : id(id), rightExpr(rightExpr) {}
    
    ExprPtr id, rightExpr;
    
    Type evalType() override;
    void codeGenerator(Code &context) override;
};

class IntExpr : public Expr {
public:
    IntExpr(int value) 
    : value(value) {}
    
    int value;
    
    Type evalType() override;
    void codeGenerator(Code &context) override;
};

class FloatExpr : public Expr {
public:
    FloatExpr(float value) 
    : value(value) {}
    
    float value;
    
    Type evalType() override;
    void codeGenerator(Code &context) override;
};

class IdentExpr : public Expr {
public:
    IdentExpr(std::string id) 
    : id(id) {}
    
    std::string id;
    
    Type evalType() override;
    void codeGenerator(Code &context) override;
};

class StringExpr : public Expr {
public:
    StringExpr(std::string value) 
    : value(value) {}
    
    std::string value;
    
    Type evalType() override;
    void codeGenerator(Code &context) override;
};

class BoolExpr : public Expr {
public:
    BoolExpr(bool flag) 
    : flag(flag) {}
    
    bool flag;
    
    Type evalType() override;
    void codeGenerator(Code &context) override;
};

class MethodInvocation : public Expr {
public:
    MethodInvocation(IdentExpr *id, ArgumentList *args) 
    : id(id), args(args) {}
    
    IdentExpr *id;
    ArgumentList *args;
    
    Type evalType() override;
    void codeGenerator(Code &context) override;
};

#define BINARY_EXPR(name) \
class name##Expr : public Expr { \
public: \
    name##Expr(ExprPtr leftExpr, ExprPtr rightExpr) \
    : leftExpr(std::move(leftExpr)), rightExpr(std::move(rightExpr)) {} \
    ExprPtr leftExpr, rightExpr; \
    Type evalType() override; \
    void codeGenerator(Code &context) override; \
}; \

#define UNARY_EXPR(name) \
class name##Expr : public Expr { \
public: \
    name##Expr(ExprPtr rightExpr) \
    : rightExpr(std::move(rightExpr)) {} \
    ExprPtr rightExpr; \
    Type evalType() override; \
    void codeGenerator(Code &context) override; \
}; \

BINARY_EXPR(Add);
BINARY_EXPR(Sub);
BINARY_EXPR(Mult);
BINARY_EXPR(Div);
BINARY_EXPR(Mod);
BINARY_EXPR(Pwr);
BINARY_EXPR(Gte);
BINARY_EXPR(Gt);
BINARY_EXPR(Lte);
BINARY_EXPR(Lt);
BINARY_EXPR(Equals);
BINARY_EXPR(NotEqual);
BINARY_EXPR(And);
BINARY_EXPR(Or);
BINARY_EXPR(PlusAssign);
BINARY_EXPR(MinusAssign);
UNARY_EXPR(Not);

class ArrayExpr : public Expr {
public:
    ArrayExpr(IdentExpr *id, ArgumentList *indexExpr) 
    : id(id), indexExpr(indexExpr) {}
    
    IdentExpr *id;
    ArgumentList *indexExpr;
    
    Type evalType() override;
    void codeGenerator(Code &context) override;
};

/**
 * @brief Stamements 
 * 
**/
class Stmt : public AstNode {
public:
    Stmt() {}
    virtual ~Stmt() {}
    int line;
    virtual void execSemantics() = 0;
    virtual std::string generateCode() = 0;
};

class BlockStmt : public Stmt {
public:
    BlockStmt(StmtList *stmtList) 
    : stmtList(stmtList) { }
    
    StmtList *stmtList;
    
    void execSemantics() override;
    std::string generateCode() override;
};

class AssignStmt : public Stmt {
public:
    AssignStmt(IdentExpr *id, ExprPtr rightExpr) 
    : id(id), rightExpr(rightExpr) {}
    
    IdentExpr *id;
    ExprPtr rightExpr;
    
    void execSemantics() override;
    std::string generateCode() override;
};

class PrintStmt : public Stmt {
public:
    PrintStmt(ExprPtr expr)
    : expr(expr) {}
    
    ExprPtr expr;
    
    void execSemantics() override;
    std::string generateCode() override;
};

class ReturnStmt : public Stmt {
public:
    ReturnStmt(ExprPtr expr) 
    : expr(expr) {}
    
    ExprPtr expr;
    
    void execSemantics() override;
    std::string generateCode() override;
};

class BreakStmt : public Stmt {
public:
    BreakStmt() {}
    
    void execSemantics() override;
    std::string generateCode() override;
};

class ContinueStmt : public Stmt {
public:
    ContinueStmt() {}
    
    void execSemantics() override;
    std::string generateCode() override;
};

class Declarator {
public:
    Declarator(std::string id, ExprPtr arrayDeclaration, bool isArray) 
    : id(id), arrarDeclaration(arrayDeclaration), isArray(isArray) {}

    std::string id;
    ExprPtr arrarDeclaration;
    bool isArray;
};

class Parameter {
public:
    Parameter(Type type, Declarator *declarator) 
    : type(type), declarator(declarator) {}

    Type type;
    Declarator *declarator;
};

class MethodDeclaration : public Stmt {
public:
    MethodDeclaration(std::string id, ParameterList *parameters, Type type, StmtList *stmtList) 
    : id(id), parameters(parameters), type(type), stmtList(stmtList) {}
    
    std::string id;
    ParameterList *parameters;
    Type type;
    StmtList *stmtList;

    void execSemantics() override;
    std::string generateCode() override;
};

class InitDeclaration {
public:
    InitDeclaration(Declarator *declarator, InitializerList *initializer) 
    : declarator(declarator), initializer(initializer) {}
    int line;
    Declarator *declarator; 
    InitializerList * initializer;
};

class Declaration : public Stmt {
public:
    Declaration(Type type, InitDeclarationList *declarations) 
    : type(type), declarations(declarations) {}
    int line;
    Type type; 
    InitDeclarationList *declarations;

    void execSemantics() override;
    std::string generateCode() override;
};

class IfStmt : public Stmt {
public:
    IfStmt(ExprPtr conditionExpr, StmtList *tStatement, StmtList *fStatement) 
    : conditionExpr(conditionExpr), tStatement(tStatement) , fStatement(fStatement) {}
    
    ExprPtr conditionExpr;
    StmtList *tStatement, *fStatement;
    
    void execSemantics() override;
    std::string generateCode() override;
};

class WhileStmt : public Stmt {
public:
    WhileStmt(ExprPtr conditionExpr, StmtList *stmtList) 
    : conditionExpr(conditionExpr), stmtList(stmtList) {}
    
    ExprPtr conditionExpr;
    StmtList *stmtList;
    
    void execSemantics() override;
    std::string generateCode() override;
};

class ForStmt : public Stmt {
public:
    ForStmt(Declaration *decl, ExprPtr expr, ExprPtr assignOpExpr, StmtList *stmtList) 
    : decl(decl), assignOpExpr(assignOpExpr), expr(expr), stmtList(stmtList) {}
    
    Declaration *decl;
    ExprPtr expr;  
    ExprPtr assignOpExpr;
    StmtList *stmtList;
    
    void execSemantics() override;
    std::string generateCode() override;
};

class ExprStmt : public Stmt {
public:
    ExprStmt(ExprPtr expr) 
    : expr(expr) {}
    
    ExprPtr expr;
    
    void execSemantics() override;
    std::string generateCode() override;
};

#endif