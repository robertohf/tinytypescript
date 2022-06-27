#include "ast.h"
#include <stack>
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <map>
#include <list>

/**
 * @brief Register Management Functions
 * 
**/
namespace Registers {
    #define INT_REGISTER_COUNT 10
    #define FLOAT_REGISTER_COUNT 32                                         

    const char *intRegisters[] = { 
                            "$t0", "$t1", "$t2", "$t3", "$t4", 
                            "$t5", "$t6", "$t7", "$t8", "$t9"
                        };

    const char *floatRegisters[] = { 
                                "$f0", "$f1", "$f2", "$f3", 
                                "$f4", "$f5", "$f6", "$f7", 
                                "$f8", "$f9", "$f10", "$f11",
                                "$f12", "$f13", "$f14", "$f15", 
                                "$f16", "$f17", "$f18", "$f19", 
                                "$f20", "$f21", "$f22", "$f23", 
                                "$f24", "$f25", "$f26", "$f27", 
                                "$f28", "$f29", "$f30", "$f31"
                            };

    /* Registers Being Used */
    std::set<std::string> intRegisterSet;
    std::set<std::string> floatRegisterSet;

    /* Releases Unused Register */
    void releaseRegister(std::string temp) {
        intRegisterSet.erase(temp);
        floatRegisterSet.erase(temp);
    }

    namespace Integer {
        /* Gets Available Int Register */
        std::string getRegister() {
            for(int i = 0; i < INT_REGISTER_COUNT; i++) {
                /* Check If Register Is Not Being Used */
                if(intRegisterSet.find(intRegisters[i]) == intRegisterSet.end()) {
                    intRegisterSet.insert(intRegisters[i]);
                    
                    return std::string(intRegisters[i]);
                }
            }

            return "";
        }
    };

    namespace Float {
        /* Gets Available Float Register */
        std::string getRegister() {
            for(int i = 0; i < FLOAT_REGISTER_COUNT; i++) {
                /* Check If Register Is Not Being Used */
                if(floatRegisterSet.find(floatRegisters[i]) == floatRegisterSet.end()) {
                    floatRegisterSet.insert(floatRegisters[i]);

                    return std::string(floatRegisters[i]);
                }
            }

            return "";
        }

        /* Casts Type to Float */
        void castRegister(Code &context) {
            if(context.type == INT) {
                std::stringstream code;
                std::string fTemp = Float::getRegister();

                code << context.code << "mtc1 " << context.registerAt << ", " << fTemp << std::endl << "cvt.s.w " << ", " << fTemp << std::endl;
                releaseRegister(context.registerAt);
                
                context.code = code.str();
                context.registerAt = fTemp;
                context.type = FLOAT;
            }
        }
    };
};

/**
 * @brief Label Management Functions
 * 
**/
namespace Label {
    int labelCounter = 0;

    /* Creates A New Label From Given Prefix */
    std::string newLabel(std::string prefix) {
        std::stringstream code;

        code << prefix << "_" << labelCounter;
        labelCounter++;

        return code.str();
    }
};

/**
 * @brief Helper Functions
 * 
**/

namespace Helper {
    class MethodInfo{
    public:
        Type returnType;
        std::list<Parameter *> *parameters;
    };

    class CodeGenVariableInfo {
    public:
        CodeGenVariableInfo(int offset, bool isArray, bool isParameter, Type type) 
        : offset(offset), isArray(isArray), isParameter(isParameter), type(type) {}

        int offset;
        bool isArray, isParameter;
        Type type;
    };

    int globalStackPointer = 0;
    std::map<std::string, Type> globalVariables = {};
    std::map<std::string, MethodInfo *> methods = {};
    std::map<std::string, CodeGenVariableInfo *> codeGenVariables = {};

    namespace Integer {
        std::unordered_map<std::string, std::string> logicalOpMap = {
                                                                        {"||", "beq "}, {"&&", "bne "},
                                                                        {"|", "beq "}, {"&", "bne "}
                                                                    };
        std::unordered_map<std::string, std::string> equalityOpMap = {
                                                                        {"==", "beq "}, {"!=", "bne "}
                                                                    };
        std::unordered_map<std::string, std::string> relativeOpMap = {
                                                                        {">=", "sge "}, {">", "sgt "},
                                                                        {"<=", "sle "}, {"<", "slt "}
                                                                    };
        std::unordered_map<std::string, std::string> arithmeticOpMap = {
                                                                        {"+", "add "}, {"-", "sub "},
                                                                        {"*", "mult "}, {"/", "div "},
                                                                        {"%", "div "}
                                                                    };

        /* Chooses What (Int) Logical Operation To Do Based On the Operator */
        std::string logicalOp(Code &context, Code &lhs, Code &rhs, std::string op) { 
            std::stringstream code;
            context.registerAt = Registers::Integer::getRegister(); 
            std::string logicalOperation = logicalOpMap[op];
            
            /* We Will Need A Label And A Temp If Condition Is Not Met */
            std::string label1 = Label::newLabel("true_logical");
            std::string label2 = Label::newLabel("false_logical"); 
            std::string iTemp = Registers::Integer::getRegister();

            if((op.compare("||") == 0)|| (op.compare("|") == 0)) {
                code << "li " << iTemp << ", 1" << std::endl;
                code << logicalOperation << lhs.registerAt << ", " << iTemp << label1 << std::endl;
                code << logicalOperation << rhs.registerAt << ", " << iTemp << label1 << std::endl;
                code << label2 << ": " << std::endl;
                code << "li " << iTemp << ", 0" << std::endl;
                code << label1 << ": " << std::endl;
            } else {
                code << "li " << iTemp << ", 1" << std::endl;
                code << logicalOperation << lhs.registerAt << ", " << iTemp << label1 << std::endl;
                code << logicalOperation << rhs.registerAt << ", " << iTemp << label1 << std::endl;
                code << label2 << ": " << std::endl;
                code << "li " << iTemp << ", 0" << std::endl;
                code << label1 << ": " << std::endl;  
            }

            context.registerAt = iTemp;
            return code.str(); 
        }

        /* Chooses What (Int) Equality Operation To Do Based On the Operator */
        std::string equalityOp(Code &context, Code &lhs, Code &rhs, std::string op) { 
            std::stringstream code;
            context.registerAt = Registers::Integer::getRegister();
            std::string equalityOperation = equalityOpMap[op];

            /* We Will Need A Label And A Temp If Condition Is Not Met */
            std::string label1 = Label::newLabel("equal");
            std::string label2 = Label::newLabel("not_equal"); 
            std::string iTemp = Registers::Integer::getRegister();

            if(op.compare("==") == 0) {
                code << equalityOperation << lhs.registerAt << ", " << rhs.registerAt << ", " << label1 << std::endl;
                code << "li " << iTemp << ", 0" << std::endl;
                code << "j " << label2 << std::endl;
                code << label1 << ": " << std::endl;
                code << "li " << iTemp << ", 1" << std::endl;
                code << label2 << ": " << std::endl;
                
            } else if(op.compare("!=") == 0){
                code << equalityOperation << lhs.registerAt << ", " << rhs.registerAt << ", " << label2 << std::endl;
                code << "li " << iTemp << ", 0" << std::endl;
                code << "j " << label1 << std::endl;
                code << label2 << ": " << std::endl;
                code << "li " << iTemp << ", 1" << std::endl;
                code << label1 << ": " << std::endl;
            }

            context.registerAt = iTemp;
            return code.str(); 
        }

        /* Chooses What (Int) Equality Operation To Do Based On the Operator */
        std::string relativeOp(Code &context, Code &lhs, Code &rhs, std::string op) { 
            std::stringstream code;
            context.registerAt = Registers::Integer::getRegister();
            std::string relativeOperation = relativeOpMap[op];

            /* We Will Need A Label And A Temp If Condition Is Not Met */
            std::string iTemp = Registers::Integer::getRegister();

            if(op.compare(">") == 0) {
                code << relativeOperation << iTemp << " " << lhs.registerAt << ", " << rhs.registerAt << std::endl;
            } else if(op.compare("<") == 0) {
                code << relativeOperation << iTemp << " " << lhs.registerAt << ", " << rhs.registerAt << std::endl;
            } else if((op.compare(">=") == 0)) {
                code << relativeOperation << iTemp << " " << lhs.registerAt << ", " << rhs.registerAt << std::endl;
            } else if((op.compare("<=") == 0)) {
                code << relativeOperation << iTemp << " " << lhs.registerAt << ", " << rhs.registerAt << std::endl;
            }

            context.registerAt = iTemp;
            return code.str(); 
        }

        /* Chooses What (Int) Arithmetic Operation To Do Based On the Operator */
        std::string arithmeticOp(Code &context, Code &lhs, Code &rhs, std::string op) { 
            std::stringstream code;
            context.registerAt = Registers::Integer::getRegister(); 
            std::string arithmeticOperation = arithmeticOpMap[op];
            
            if((op.compare("+") == 0) || (op.compare("-") == 0)) {
                code << arithmeticOperation << context.registerAt << ", " << lhs.registerAt << ", " << rhs.registerAt << std::endl;
            } else if((op.compare("*") == 0) || (op.compare("/") == 0)) {
                code << arithmeticOperation << lhs.registerAt << ", " << rhs.registerAt << std::endl
                << "mflo " << context.registerAt;
            } else if(op.compare("%") == 0) {
                code << arithmeticOperation << lhs.registerAt << ", " << rhs.registerAt << std::endl 
                << "mfhi " << context.registerAt;
            }

            return code.str(); 
        }
    };

    namespace Float {
        std::unordered_map<std::string, std::string> equalityOpMap = {
                                                                        {"==", "c.eq.s "}, {"!=", "c.ne.s "}
                                                                    };
        std::unordered_map<std::string, std::string> relativeOpMap = {
                                                                        {">", "c.lt.s "}, {"<", "c.gt.s "},
                                                                        {">", "c.le.s "}, {"<", "c.ge.s "}
                                                                    };
        std::unordered_map<std::string, std::string> arithmeticOpMap = {
                                                                        {"+", "add.s "}, {"-", "sub.s "},
                                                                        {"*", "mul.s "}, {"/", "div.s "}
                                                                    };

        /* Chooses What (Float) Equality Operation To Do Based On the Operator */
        std::string equalityOp(Code &context, Code &lhs, Code &rhs, std::string op) { 
            std::stringstream code;
            context.registerAt = Registers::Float::getRegister();
            std::string equalityOperation = equalityOpMap[op];

            /* We Will Need A Label And A Temp If Condition Is Not Met */
            std::string label1 = Label::newLabel("equal");
            std::string label2 = Label::newLabel("not_equal"); 
            std::string fTemp = Registers::Float::getRegister();

            if(op.compare("==") == 0) {
                code << equalityOperation << lhs.registerAt << ", " << rhs.registerAt << std::endl;
                code << "bc1t" << label1;
                code << "li.s " << fTemp << ", 0" << std::endl;
                code << "j " << label2 << std::endl;
                code << label1 << ": " << std::endl;
                code << "li.s " << fTemp << ", 1" << std::endl;
                code << label2 << ": " << std::endl;
            } else {
                code << equalityOperation << lhs.registerAt << ", " << rhs.registerAt << ", " << label2;
                code << "bc1t" << label1;
                code << "li.s " << fTemp << ", 0" << std::endl;
                code << "j " << label1 << std::endl;
                code << label2 << ": " << std::endl;
                code << "li.s " << fTemp << ", 1" << std::endl;
                code << label1 << ": " << std::endl;
            }

            context.registerAt = fTemp;
            return code.str(); 
        }

        /* Chooses What (Float) Equality Operation To Do Based On the Operator */
        std::string relativeOp(Code &context, Code &lhs, Code &rhs, std::string op) { 
            std::stringstream code;
            context.registerAt = Registers::Float::getRegister();
            std::string relativeOperation = relativeOpMap[op];

            /* We Will Need A Label And A Temp If Condition Is Not Met */
            // std::string label1 = Label::newLabel("greater_than_equal");
            // std::string label2 = Label::newLabel("less_than_equal"); 
            std::string fTemp = Registers::Float::getRegister();

            if(op.compare(">") == 0) {
                code << relativeOperation << lhs.registerAt << ", " << rhs.registerAt << ", " << std::endl;
            } else if(op.compare("<") == 0) {
                code << relativeOperation << lhs.registerAt << ", " << rhs.registerAt << ", " << std::endl;
            }

            context.registerAt = fTemp;
            return code.str(); 
        }

        /* Chooses What (Float) Arithmetic Operation To Do Based On the Operator */
        std::string arithmeticOp(Code &context, Code &lhs, Code &rhs, std::string op) { 
            std::stringstream code; 
            context.registerAt = Registers::Float::getRegister(); 
            std::string arithmeticOperation = arithmeticOpMap[op];
            
            if((op.compare("+") == 0) || (op.compare("-") == 0))
                code << arithmeticOperation << context.registerAt << ", " << lhs.registerAt << ", " << rhs.registerAt;
            else
                code << arithmeticOperation << lhs.registerAt << ", " << rhs.registerAt << std::endl << "mflo " << context.registerAt;
            
            return code.str(); 
        }
    };
};

class ContextStack{
    public:
        struct ContextStack *prev;
        std::map<std::string, Type> variables;
    };

ContextStack * context = NULL;

Type getLocalVariabletype(std::string id){
    ContextStack *currContext = context;
    while(currContext != NULL){
        if(currContext->variables[id] != 0){
            return currContext->variables[id];
        }
        currContext = currContext->prev;
    }
    if(!context->variables.empty()){
        return context->variables[id];
    }
    return INVALID;
}

/**
 * @brief Expression Functions
 * 
**/
Type AssignExpr::evalType() { 
    return INT; 
}

void AssignExpr::codeGenerator(Code &context) {
    Code rhs;
    std::stringstream code;

    this->rightExpr->codeGenerator(rhs);
    code << rhs.code << std::endl;
    Registers::releaseRegister(rhs.registerAt);

    std::string name = ((IdentExpr *)this->id)->id;
    if(Helper::codeGenVariables.find(name) == Helper::codeGenVariables.end()) {
        if(rhs.type == INT) {
            code << "sw " << rhs.registerAt << ", " << name << std::endl;
        } else if(rhs.type == FLOAT) {
            code << "s.s " << rhs.registerAt << ", " << name << std::endl; 
        }
    } else {
        if(rhs.type == INT) {
            code << "sw " << rhs.registerAt << " " << Helper::codeGenVariables[name]->offset << "($sp)" << std::endl;
        } else if(rhs.type == FLOAT) {
            code << "s.s " << rhs.registerAt << " " << Helper::codeGenVariables[name]->offset << "($sp)" << std::endl; 
        }
    }

    Registers::releaseRegister(rhs.registerAt);
    context.code = code.str();
}

Type IntExpr::evalType() {
    return INT;
}

void IntExpr::codeGenerator(Code &context) {
    std::stringstream code;
    std::string iTemp = Registers::Integer::getRegister();
    
    code << "li " << iTemp << ", " << this->value << std::endl;

    context.code = code.str();
    context.registerAt = iTemp;
    context.type = INT;
}

Type FloatExpr::evalType() {
    return FLOAT;
}

void FloatExpr::codeGenerator(Code &context) {
    std::stringstream code;
    std::string fTemp = Registers::Float::getRegister();

    code << "li.s" << fTemp << ", " << this->value << std::endl;

    context.code = "";
    context.registerAt = fTemp;
    context.type = FLOAT;
}

Type IdentExpr::evalType() { 
    Type value;

    if(context != NULL){
        value = getLocalVariabletype(this->id);
        if(value != 0){
            return value;
        }
    }
    value = getLocalVariabletype(this->id);
    if(value == 0){
        std::cout<<"Error: '" << this->id << "' was not declared in this scope line: " << this->line <<std::endl;
        exit(0);
    }
    return value;
}

void IdentExpr::codeGenerator(Code &context) {
    /* If Its Not On The Dictionary Then Its a Global Variable */
    if(Helper::codeGenVariables.find(this->id) == Helper::codeGenVariables.end()) {
        context.type = Helper::globalVariables[this->id];

        if(Helper::globalVariables[this->id] == INT) {
            std::string iTemp = Registers::Integer::getRegister();
            
            context.registerAt = iTemp;
            context.code = "lw " + iTemp + ", " + this->id + "\n";
        } else if(Helper::globalVariables[this->id] == FLOAT) {
            std::string fTemp = Registers::Float::getRegister();

            context.registerAt = fTemp;
            context.code = "l.s " + fTemp + ", " + this->id + "\n";
        }
    } else {
        context.type = Helper::codeGenVariables[this->id]->type;

        if(Helper::codeGenVariables[this->id]->type == INT && !Helper::codeGenVariables[this->id]->isArray) {
            std::string iTemp = Registers::Integer::getRegister();
            
            context.registerAt = iTemp;
            context.code = "lw " + iTemp + ", " + std::to_string(Helper::codeGenVariables[this->id]->offset) + "($sp)\n";
        } else if(Helper::codeGenVariables[this->id]->type == FLOAT && !Helper::codeGenVariables[this->id]->isArray) {
            std::string fTemp = Registers::Float::getRegister();
            
            context.registerAt = fTemp;
            context.code = "l.s " + fTemp + ", " + std::to_string(Helper::codeGenVariables[this->id]->offset) + "($sp)\n";
        }
    }
}

Type StringExpr::evalType() {
    return STRING;
}

void StringExpr::codeGenerator(Code &context) {
    std::stringstream code;
    std::string strLabel = Label::newLabel("string");

    code << strLabel << ": .asciiz " << this->value << std::endl;
    
    context.code = "";
    context.registerAt = strLabel;
    context.type = STRING;
    assemblyFile.data += code.str();
}

Type BoolExpr::evalType() {
    return BOOL;
}

void BoolExpr::codeGenerator(Code &context) {}

Type MethodInvocation::evalType() {
    Helper::MethodInfo *method = Helper::methods[this->id->id];
    
    if(method == NULL) {
        std::cerr<<"Error: invocation of undefined method: " << this->id->id << std::endl;
        exit(0);
    }

    if(method->parameters->size() > this->args->size()) {
        std::cerr<<"Error: too few arguments for method: " << this->id->id << std::endl;
        exit(0);
    }

    if(method->parameters->size() < this->args->size()) {
        std::cerr<<"Error: too many arguments for method: "<<this->id->id << std::endl;
        exit(0);
    }
    
    return method->returnType;
 }

void MethodInvocation::codeGenerator(Code &context) {
    std::list<Code> codes;
    Code argsCode;
    std::stringstream code;
    std::list<Expr *>::iterator exprIt = this->args->begin();

    while(exprIt != this->args->end()) {
        (*exprIt)->codeGenerator(argsCode);
        code << argsCode.code << std::endl;
        codes.push_back(argsCode);

        exprIt++;
    }

    int i = 0;
    std::list<Code>::iterator codeIt = codes.begin();
    while(codeIt != codes.end()) {

        Registers::releaseRegister((*codeIt).registerAt);
        if((*codeIt).type == INT) {
            code << "move $a" << i << ", " << (*codeIt).registerAt << std::endl;
        } else if((*codeIt).type == FLOAT) {
            code << "mfc1 $a" << i << ", " << (*codeIt).registerAt << std::endl;
        }
        i++;
        codeIt++;
    }

    code << "jal " << this->id->id << std::endl;
    std::string result;

    Helper::methods[this->id->id]->returnType = this->evalType();

    if(Helper::methods[this->id->id]->returnType == INT) {
        result = Registers::Integer::getRegister();
        code << "move " << result << ", $v0" << std::endl;
    } else if(Helper::methods[this->id->id]->returnType == FLOAT) {
        result = Registers::Float::getRegister();
        code << "mtc1 $v0, " << result << std::endl;
    }


    context.code = code.str();
    context.registerAt = result;
    context.type = Helper::methods[this->id->id]->returnType;
}

void PlusAssignExpr::codeGenerator(Code &context){

}

void MinusAssignExpr::codeGenerator(Code &context){

}

#define BINARY_EXPR_EVAL_TYPE(name) \
Type name##Expr::evalType() { \
    return INT; \
} \

BINARY_EXPR_EVAL_TYPE(And);
BINARY_EXPR_EVAL_TYPE(Or);
BINARY_EXPR_EVAL_TYPE(Add);
BINARY_EXPR_EVAL_TYPE(Sub);
BINARY_EXPR_EVAL_TYPE(Mult);
BINARY_EXPR_EVAL_TYPE(Div);
BINARY_EXPR_EVAL_TYPE(Mod);
BINARY_EXPR_EVAL_TYPE(Pwr);
BINARY_EXPR_EVAL_TYPE(Gte);
BINARY_EXPR_EVAL_TYPE(Gt);
BINARY_EXPR_EVAL_TYPE(Lt);
BINARY_EXPR_EVAL_TYPE(Lte);
BINARY_EXPR_EVAL_TYPE(Equals);
BINARY_EXPR_EVAL_TYPE(NotEqual);
BINARY_EXPR_EVAL_TYPE(Not);

#define LOGICAL_EXPR_CODE_GEN(name, op) \
void name##Expr::codeGenerator(Code &context) { \
    Code lhs, rhs; \
    std::stringstream code; \
    this->leftExpr->codeGenerator(lhs); \
    this->rightExpr->codeGenerator(rhs); \
    if(lhs.type == INT && rhs.type == INT) { \
        context.type = INT; \
        Registers::releaseRegister(lhs.registerAt); \
        Registers::releaseRegister(rhs.registerAt); \
        code << lhs.code << rhs.code << Helper::Integer::logicalOp(context, lhs, rhs, op); \
    } else { \
        context.type = FLOAT; \
        Registers::Float::castRegister(lhs); \
        Registers::Float::castRegister(rhs); \
        Registers::releaseRegister(lhs.registerAt); \
        Registers::releaseRegister(rhs.registerAt); \
        code << lhs.code << rhs.code; \
    } \
    context.code += code.str(); \
} \

#define EQUALITY_EXPR_CODE_GEN(name, op) \
void name##Expr::codeGenerator(Code &context) { \
    Code lhs, rhs; \
    std::stringstream code; \
    this->leftExpr->codeGenerator(lhs); \
    this->rightExpr->codeGenerator(rhs); \
    if(lhs.type == INT && rhs.type == INT) { \
        context.type = INT; \
        Registers::releaseRegister(lhs.registerAt); \
        Registers::releaseRegister(rhs.registerAt); \
        code << lhs.code << rhs.code << Helper::Integer::equalityOp(context, lhs, rhs, op); \
    } else { \
        context.type = FLOAT; \
        Registers::Float::castRegister(lhs); \
        Registers::Float::castRegister(rhs); \
        Registers::releaseRegister(lhs.registerAt); \
        Registers::releaseRegister(rhs.registerAt); \
        code << lhs.code << rhs.code << Helper::Float::equalityOp(context, lhs, rhs, op); \
    } \
    context.code += code.str(); \
} \

#define RELATIVE_EXPR_CODE_GEN(name, op) \
void name##Expr::codeGenerator(Code &context) { \
    Code lhs, rhs; \
    std::stringstream code; \
    this->leftExpr->codeGenerator(lhs); \
    this->rightExpr->codeGenerator(rhs); \
    if(lhs.type == INT && rhs.type == INT) { \
        context.type = INT; \
        Registers::releaseRegister(lhs.registerAt); \
        Registers::releaseRegister(rhs.registerAt); \
        code << lhs.code << rhs.code << Helper::Integer::relativeOp(context, lhs, rhs, op); \
    } else { \
        context.type = FLOAT; \
        Registers::Float::castRegister(lhs); \
        Registers::Float::castRegister(rhs); \
        Registers::releaseRegister(lhs.registerAt); \
        Registers::releaseRegister(rhs.registerAt); \
        code << lhs.code << rhs.code << Helper::Float::relativeOp(context, lhs, rhs, op); \
    } \
    context.code += code.str(); \
} \

#define ARITHMETIC_EXPR_CODE_GEN(name, op) \
void name##Expr::codeGenerator(Code &context) { \
    Code lhs, rhs; \
    std::stringstream code; \
    this->leftExpr->codeGenerator(lhs); \
    this->rightExpr->codeGenerator(rhs); \
    if(lhs.type == INT && rhs.type == INT) { \
        context.type = INT; \
        Registers::releaseRegister(lhs.registerAt); \
        Registers::releaseRegister(rhs.registerAt); \
        code << lhs.code << rhs.code << Helper::Integer::arithmeticOp(context, lhs, rhs, op) << std::endl; \
    } else { \
        context.type = FLOAT; \
        Registers::Float::castRegister(lhs); \
        Registers::Float::castRegister(rhs); \
        Registers::releaseRegister(lhs.registerAt); \
        Registers::releaseRegister(rhs.registerAt); \
        code << lhs.code << rhs.code << Helper::Float::arithmeticOp(context, lhs, rhs, op) << std::endl; \
    } \
    context.code += code.str(); \
} \

#define UNARY_EXPR_CODE_GEN(name, op) \
void name##Expr::codeGenerator(Code &context) { \
    Code rhs; \
    std::stringstream code; \
    this->rightExpr->codeGenerator(rhs); \
    if(rhs.type == INT) { \
        context.type = INT; \
        Registers::releaseRegister(rhs.registerAt); \
        code << rhs.code << std::endl; \
    } else { \
        context.type = FLOAT; \
        Registers::Float::castRegister(rhs); \
        Registers::releaseRegister(rhs.registerAt); \
        code << rhs.code << std::endl; \
    } \
} \

LOGICAL_EXPR_CODE_GEN(And, "&");
LOGICAL_EXPR_CODE_GEN(Or, "|");
EQUALITY_EXPR_CODE_GEN(Equals, "==");
EQUALITY_EXPR_CODE_GEN(NotEqual, "!=");
RELATIVE_EXPR_CODE_GEN(Gte, ">=");
RELATIVE_EXPR_CODE_GEN(Gt, ">");
RELATIVE_EXPR_CODE_GEN(Lt, "<");
RELATIVE_EXPR_CODE_GEN(Lte, "<=");
ARITHMETIC_EXPR_CODE_GEN(Add, "+");
ARITHMETIC_EXPR_CODE_GEN(Sub, "-");
ARITHMETIC_EXPR_CODE_GEN(Mult, "*");
ARITHMETIC_EXPR_CODE_GEN(Div, "/");
ARITHMETIC_EXPR_CODE_GEN(Mod, "%");
ARITHMETIC_EXPR_CODE_GEN(Pwr, "^");
UNARY_EXPR_CODE_GEN(Not, "!");

Type ArrayExpr::evalType() {
/*
    int cont = 0;
    Type idType = this->id->evalType();

    std::list<Expr *>::iterator argsIt = this->indexExpr->begin();
    while(argsIt != this->indexExpr.end()){
        Expr *id = *argsIt;
        if(id->evalType() != idType){
            std::cout << "Error: Array " << this->id << "expression type does not match array type. Line: " << this->line <<std::endl;
            exit(0);
        }
        argsIt++;
    }
    return this->id->evalType();
    */
}

void ArrayExpr::codeGenerator(Code &context) {}

/**
 * @brief Statement Functions
 * 
**/
void BlockStmt::execSemantics() {}

std::string BlockStmt::generateCode() { 
    std::stringstream code;

    for(auto it : *stmtList)
        code << it->generateCode();

    return code.str(); 
}

void AssignStmt::execSemantics() {}

std::string AssignStmt::generateCode() {
    std::stringstream code;
    Code rhs;

    this->rightExpr->codeGenerator(rhs);
    code << rhs.code << std::endl;

    std::string name = ((IdentExpr *)this->id)->id;
    if (Helper::codeGenVariables.find(name) == Helper::codeGenVariables.end()) {
        if(rhs.type == INT) {
            code << "sw " << rhs.registerAt << ", " << name << std::endl;
        } else if(rhs.type == FLOAT) {
            code << "s.s " << rhs.registerAt << ", " << name << std::endl; 
        }
    } else {
        if(rhs.type == INT) {
            code << "sw " << Helper::codeGenVariables[name]->offset << "($sp)" << std::endl;
        } else if(rhs.type == FLOAT) {
            code << "s.s " << Helper::codeGenVariables[name]->offset << "($sp)" << std::endl; 
        }
    }

    Registers::releaseRegister(rhs.registerAt);
    return code.str(); 
}

void ReturnStmt::execSemantics() {}

std::string ReturnStmt::generateCode() { 
    Code exprContext;
    std::stringstream code;

    this->expr->codeGenerator(exprContext);
    Registers::releaseRegister(exprContext.code);

    code << exprContext.code << std::endl;
    if(exprContext.type == INT) {
        code << "move $v0, " << exprContext.registerAt << std::endl;
    } else if(exprContext.type == FLOAT) {
        code << "mfc1 $v0, " << exprContext.registerAt << std::endl;
    }

    return code.str();  
}

void BreakStmt::execSemantics() {}

std::string BreakStmt::generateCode() { 
    std::stringstream code;

    code << "j " << std::endl;
    return code.str(); 
}

void ContinueStmt::execSemantics() {}

std::string ContinueStmt::generateCode() { return ""; }

void PrintStmt::execSemantics() {}

std::string PrintStmt::generateCode() {
    std::stringstream code;
    Code exprContext;
    
    this->expr->codeGenerator(exprContext);
    Registers::releaseRegister(exprContext.registerAt);
    code << exprContext.code << std::endl;
    
    if(exprContext.type == INT) {
        code << "move $a0, " << exprContext.registerAt << std::endl
        << "li $v0, 1" << std::endl
        << "syscall" << std::endl;
    } else if(exprContext.type == FLOAT) {
        code << "mov.s $f12, " << exprContext.registerAt << std::endl
        << "li $v0, 2" << std::endl
        << "syscall" << std::endl;
    } else if (exprContext.type == STRING) {
        code << "la $a0, " << exprContext.registerAt << std::endl
        << "li $v0, 4" << std::endl
        << "syscall" << std::endl;
    }

    return code.str(); 
}

void MethodDeclaration::execSemantics() {
    if(this->parameters != nullptr) {
        if (this->parameters->size() > 4) {
            std::cerr<<"Error: only 4 paramaters can be used " << std::endl;
            exit(0);
        }
    }

    if (Helper::methods[this->id] != 0) {
        std::cout<<"Error: redefinition of method "<< std::endl;
        exit(0);
    }

    Helper::methods[this->id] = new Helper::MethodInfo();
    Helper::methods[this->id]->returnType = this->type;
    Helper::methods[this->id]->parameters = this->parameters;
}

std::string storeState() {
    std::stringstream code;

    code << "sw $ra, " << Helper::globalStackPointer << "($sp)" << std::endl;
    Helper::globalStackPointer += 4;

    return code.str();
}

std::string loadState(std::string state) {
    std::string::size_type n = 0;
    std::string originalStatement = "sw";
    
    while ((n = state.find(originalStatement, n)) != std::string::npos) {
        state.replace(n, originalStatement.size(), "lw");
        n += originalStatement.size();
    }
    return state;
}

std::string MethodDeclaration::generateCode() {
    execSemantics();
    
    std::stringstream code;
    int stackPointer = 4;
    Helper::globalStackPointer = 0;

    code << this->id << ": " << std::endl;
    std::string state = storeState();
    code << state << std::endl;

    /* Parameters */
    if(this->parameters != nullptr) {
        if(this->parameters->size() > 0) {
            std::list<Parameter *>::iterator paramIt = this->parameters->begin();

            for(int i = 0; i < this->parameters->size(); i++) {
                code << "sw $a" << i << ", " << stackPointer << "($sp)" << std::endl;
                
                Helper::codeGenVariables[(*paramIt)->declarator->id] =
                    new Helper::CodeGenVariableInfo(stackPointer, false, true, (*paramIt)->type);
                
                stackPointer += 4;
                Helper::globalStackPointer += 4;

                paramIt++;
            }
        }
    }

    for(auto it : *stmtList)
        code << it->generateCode();

    std::stringstream sp;
    int currentStackPointer = Helper::globalStackPointer;

    sp << std::endl
    << "addiu $sp, $sp, -" << currentStackPointer << std::endl;

    code << loadState(state) << std::endl
    << "addiu $sp, $sp, " << currentStackPointer << std::endl
    << "jr $ra" << std::endl;

    Helper::codeGenVariables.clear();

    std::string result = code.str();
    result.insert(this->id.size() + 2, sp.str());

    return result; 
}

void Declaration::execSemantics() {}

std::string Declaration::generateCode() {
    std::stringstream code;
    std::list<InitDeclaration *>::iterator it = this->declarations->begin();

    while(it != this->declarations->end()) {
        InitDeclaration *declaration = *(it);
        
        if(!declaration->declarator->isArray) {
            Helper::codeGenVariables[declaration->declarator->id] 
                = new Helper::CodeGenVariableInfo(Helper::globalStackPointer, false, false, this->type);
            
            Helper::globalStackPointer+=4;
        } else {
 
        }

        if(declaration->initializer != NULL) {
            std::list<Expr *>::iterator exprIt = declaration->initializer->begin();
            int offset = Helper::codeGenVariables[declaration->declarator->id]->offset;

            for(int i = 0; i < declaration->initializer->size(); i++) {
                Code exprContext;
                (*exprIt)->codeGenerator(exprContext);
                code << exprContext.code << std::endl;

                if(exprContext.type == INT) {
                    code << "sw " << exprContext.registerAt << ", " << offset << "($sp)" << std::endl;
                } else if(exprContext.type == FLOAT) {
                    code << "s.s " << exprContext.registerAt << ", " << offset << "($sp)" << std::endl;
                }
                Registers::releaseRegister(exprContext.registerAt);
                exprIt++;
            }
        }

        it++;
    }

    return code.str();
}

void IfStmt::execSemantics() {}

std::string IfStmt::generateCode() { 
    Code exprContext;
    std::stringstream code;
    std::string end_if_label = Label::newLabel("end_if");

    this->conditionExpr->codeGenerator(exprContext);

    code << exprContext.code << std::endl;
    
    if(exprContext.type == INT || exprContext.type == BOOL) {
        code<< "beqz "<< exprContext.registerAt << ", " << end_if_label << std::endl;
    } else {
        code << "bc1f "<< end_if_label << std::endl;
    }
    
    for(auto it : *tStatement) 
        code << it->generateCode();
    
    if(fStatement != nullptr) {
        for(auto it : *fStatement) 
            code << it->generateCode();
    }

    code << end_if_label << ": " << std::endl;

    Registers::releaseRegister(exprContext.registerAt);
    
    return code.str();
 }

void WhileStmt::execSemantics() {}

std::string WhileStmt::generateCode() {
    Code exprContext;
    std::stringstream code;
    std::string label1 = Label::newLabel("while");
    std::string label2 = Label::newLabel("end_while");

    this->conditionExpr->codeGenerator(exprContext);

    code << label1 << ": " << std::endl
    << exprContext.code << std::endl;

    if(exprContext.type == INT) {
        code << "beqz " << exprContext.registerAt << ", " << label2 << std::endl;
    } else {
        code << "bc1f " << label2 << std::endl;
    }

    for(auto it : *stmtList) 
        code << it->generateCode();

    code << "j " << label1 << std::endl
    << label2 << ": " << std::endl;

    Registers::releaseRegister(exprContext.registerAt);


    return code.str(); 
}

void ForStmt::execSemantics() {}

std::string ForStmt::generateCode() {
    Code context;
    std::stringstream code;
    std::string forLabel = Label::newLabel("for");
    std::string endForLabel = Label::newLabel("end_for");
    std::string assignForLabel = Label::newLabel("assign_for");

    code << this->decl->generateCode() << std::endl;

    this->expr->codeGenerator(context);
    Registers::releaseRegister(context.registerAt);
    code << forLabel << ": " << std::endl;
    code << context.code << std::endl;
    
    if(context.type == INT) {
        code << "beqz " << context.registerAt << ", " << endForLabel << std::endl;
    } else if(context.type == BOOL) {
        code << "bc1f " << endForLabel << std::endl;
    }

    for(auto it : *stmtList) 
        code << it->generateCode();
    
    code << assignForLabel << ": " << std::endl;
    
    this->assignOpExpr->codeGenerator(context);
    Registers::releaseRegister(context.registerAt);
    code << context.code << std::endl;


    code << "j " << forLabel << std::endl;
    code << endForLabel << ": " << std::endl;

    return code.str(); 
}

void ExprStmt::execSemantics() {}

std::string ExprStmt::generateCode() {
    Code exprContext;

    this->expr->codeGenerator(exprContext);

    Registers::releaseRegister(exprContext.registerAt);

    return exprContext.code; 
}