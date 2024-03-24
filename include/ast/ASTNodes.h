#ifndef ASTNODES_H
#define ASTNODES_H

#include <string>
#include <vector>
#include <memory>

#include "symbolTable/SymbolTable.h"

class ASTNode {
    public:
        virtual ~ASTNode() = default;
};
class Declaration : public ASTNode {
    public:
};
class Expression : public ASTNode {
    public:
        virtual std::string toString() const = 0; 
        virtual std::string getType(SymbolTable& symbolTable) const = 0;
};
class Statement : public ASTNode {
    public:
};
class Function : public ASTNode {
    public:
        virtual std::string toString() const = 0;  
};

//Declarations
class IntDeclaration : public Declaration {
    public:
        std::string name;
        std::string number;

        IntDeclaration(std::string name, std::string number)
            : name(std::move(name)), number(std::move(number)) {}
};

class FloatDeclaration : public Declaration {
    public:
        std::string name;
        std::string number;

        FloatDeclaration(std::string name, std::string number)
            : name(std::move(name)), number(std::move(number)) {}
};

class StringDeclaration : public Declaration {
    public:
        std::string name;
        std::string value;

        StringDeclaration(std::string name, std::string value)
            : name(std::move(name)), value(std::move(value)) {}
};

//Expressions
class AssignmentExpression : public Expression {
    public:
        std::string name;
        std::unique_ptr<Expression> expression;

        AssignmentExpression(std::string name, std::unique_ptr<Expression> expr)
            : name(std::move(name)), expression(std::move(expr)) {}

        std::string toString() const override {
            return "aE[" + name + " = " + expression->toString() + ";" + "]";
        }

        std::string getType(SymbolTable& symbolTable) const override {
        auto symbolInfo = symbolTable.getSymbolInfo(name);
        if (!symbolInfo.has_value()) {
            throw std::runtime_error("Variable " + name + " not declared.");
        }
        // Optionally, check if the type of `expression` matches the variable's type
        return symbolInfo->type;
        }
};


class LogicOrExpression : public Expression {
    public:
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;

        LogicOrExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
            : left(std::move(left)), right(std::move(right)) {}

        std::string toString() const override {
            return "loE[" + left->toString() + " " + right->toString() + "]";
        }

        std::string getType(SymbolTable& symbolTable) const override {
            auto leftType = left->getType(symbolTable);
            auto rightType = right->getType(symbolTable);

            // Simplified example: assume all operators work with integers only
            if (leftType == "bool" && rightType == "bool") {
                return "int";
            }
            else {
                throw std::runtime_error("Type mismatch in logical or expression. Only support boolean types.");
            }
        }
};

class LogicAndExpression : public Expression {
    public:
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;

        LogicAndExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
            : left(std::move(left)), right(std::move(right)) {}

        std::string toString() const override {
            return "laE[" + left->toString() + " " + right->toString() + "]";
        }

        std::string getType(SymbolTable& symbolTable) const override {
            auto leftType = left->getType(symbolTable);
            auto rightType = right->getType(symbolTable);

            // Simplified example: assume all operators work with integers only
            if (leftType == "bool" && rightType == "bool") {
                return "int";
            }
            else {
                throw std::runtime_error("Type mismatch in logical and expression. Only support boolean types.");
            }
        }
};

class EqualityExpression : public Expression {
    public:
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        std::string op;

        EqualityExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, std::string op)
            : left(std::move(left)), right(std::move(right)), op(op) {}

        std::string toString() const override {
            return "eE[" + left->toString() + " " + op + " " + right->toString() + "]";
        }

        std::string getType(SymbolTable& symbolTable) const override {
            auto leftType = left->getType(symbolTable);
            auto rightType = right->getType(symbolTable);

            // Simplified example: assume all operators work with integers only
            if (leftType == "int" && rightType == "int") {
                return "int";
            }
            else if (leftType == "float" && rightType == "float") {
                return "float";
            }
            
            else if (leftType == "string" && rightType == "string") {
                return "string";
            }
            
            else {
                throw std::runtime_error("Type mismatch in equality expression. Only support int and float types.");
            }
        }
};

class ComparisonExpression : public Expression {
    public:
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        std::string op;

        ComparisonExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, std::string op)
            : left(std::move(left)), right(std::move(right)), op(op) {}

        std::string toString() const override {
            return "cE[" + left->toString() + " " + op + " " + right->toString() + "]";
        }

        std::string getType(SymbolTable& symbolTable) const override {
            auto leftType = left->getType(symbolTable);
            auto rightType = right->getType(symbolTable);

            // Simplified example: assume all operators work with integers only
            if (leftType == "int" && rightType == "int") {
                return "int";
            }
            else if (leftType == "float" && rightType == "float") {
                return "float";
            }
            else {
                throw std::runtime_error("Type mismatch in comparison expression. Only support int and float types.");
            }
        }
};

class TermExpression : public Expression {
    public:
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        std::string op;

        TermExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, std::string op)
            : left(std::move(left)), right(std::move(right)), op(op) {}

        std::string toString() const override {
            return "tE[" + left->toString() + " " + op + " " + right->toString() + "]";
        }

        std::string getType(SymbolTable& symbolTable) const override {
            auto leftType = left->getType(symbolTable);
            auto rightType = right->getType(symbolTable);

            // Simplified example: assume all operators work with integers only
            if (leftType == "int" && rightType == "int") {
                return "int";
            }
            else if (leftType == "float" && rightType == "float") {
                return "float";
            }
            else {
                throw std::runtime_error("Type mismatch in term expression. Only support int and float types.");
            }
        }
};

class FactorExpression : public Expression {
    public:
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        std::string op;

        FactorExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, std::string op)
            : left(std::move(left)), right(std::move(right)), op(op) {}

        std::string toString() const override {
            return "fE[" + left->toString() + " " + op + " " + right->toString() + "]";
        }

        std::string getType(SymbolTable& symbolTable) const override {
            auto leftType = left->getType(symbolTable);
            auto rightType = right->getType(symbolTable);

            // Simplified example: assume all operators work with integers only
            if (leftType == "int" && rightType == "int") {
                return "int";
            }
            else if (leftType == "float" && rightType == "float") {
                return "float";
            }
            else {
                throw std::runtime_error("Type mismatch in factor expression. Only support int and float types.");
            }
        }
};

class PrimaryExpression : public Expression {
    public:
        std::string name;
        PrimaryExpression(std::string name)
            : name(std::move(name)) {}

        std::string toString() const override {
            return "pE[" + name + "]";
        }

        std::string getType(SymbolTable& symbolTable) const override {
        auto symbolInfo = symbolTable.getSymbolInfo(name);
        if (symbolInfo.has_value()) {
            return symbolInfo->type; // Variable's type from the symbol table
        } 
        else {
            // If not found, handle literals or throw an error if it should be a declared variable
            throw std::runtime_error("PrimaryExpression " + name + " type not found.");
            }
        }

};

class BinaryExpression : public Expression {
    public:
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        std::string op;
        BinaryExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, std::string op)
            : left(std::move(left)), right(std::move(right)), op(op) {}
        
        std::string toString() const override {
            return "bE[" + left->toString() + " " + op + " " + right->toString() + "]";
        }
        
        std::string getType(SymbolTable& symbolTable) const override {
            auto leftType = left->getType(symbolTable);
            auto rightType = right->getType(symbolTable);

            // Simplified example: assume all operators work with integers only
            if (leftType == "int" && rightType == "int") {
                return "int";
            }
            else if (leftType == "float" && rightType == "float") {
                return "float";
            }
            else {
                throw std::runtime_error("Type mismatch in binary expression. Only support int and float types.");
            }
        }
};

class UnaryExpression : public Expression {
    public:
        std::unique_ptr<Expression> expr;
        std::string op;

        UnaryExpression(std::unique_ptr<Expression> expr, std::string op)
            : expr(std::move(expr)), op(op) {}

        std::string toString() const override {
            return "uE[" + op + expr->toString() + "]";
        }

        std::string getType(SymbolTable& symbolTable) const override {
        std::string exprType = expr->getType(symbolTable);

        return exprType;
        }
};

//Statements
class LoopStatement : public Statement {
    public:
        std::unique_ptr<Expression> start; // For range-based or initialization expression
        std::unique_ptr<Expression> end;   // For condition expression
        std::unique_ptr<Statement> body;

        LoopStatement(std::unique_ptr<Expression> start, std::unique_ptr<Expression> end, std::unique_ptr<Statement> body)
            : start(std::move(start)), end(std::move(end)), body(std::move(body)) {}
};

class PrintStatement : public Statement {
    public:
        std::unique_ptr<Expression> expr;
        PrintStatement(std::unique_ptr<Expression> expr)
            : expr(std::move(expr)) {}
};

class WhileLoopStatement : public Statement {
    public:
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Statement> body;
        WhileLoopStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
            : condition(std::move(condition)), body(std::move(body)) {}
};

class ForLoopStatement : public Statement {
    public:
        std::unique_ptr<Expression> start;
        std::unique_ptr<Expression> end;
        std::unique_ptr<Expression> body;
        ForLoopStatement(std::unique_ptr<Expression> start, std::unique_ptr<Expression> end, std::unique_ptr<Expression> body)
            : start(std::move(start)), end(std::move(end)), body(std::move(body)) {}
};

class AssignmentStatement : public Statement {
    public:
        std::string name;
        std::unique_ptr<Expression> expression;
        AssignmentStatement(std::string name, std::unique_ptr<Expression> expr)
            : name(std::move(name)), expression(std::move(expr)) {}
};


class IfStatement : public Statement {
    public:
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Statement> body;
        IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
            : condition(std::move(condition)), body(std::move(body)) {}
};

class ElseStatement : public Statement {
    public:
        std::unique_ptr<Statement> body;
        ElseStatement(std::unique_ptr<Statement> body)
            : body(std::move(body)) {}
};

class ReturnStatement : public Statement {
    public:
        std::unique_ptr<Expression> expression;
        ReturnStatement(std::unique_ptr<Expression> expr)
            : expression(std::move(expr)) {}
};

class BlockStatement : public Statement {
    public:
        std::vector<std::unique_ptr<Statement>> statements;

        BlockStatement(std::vector<std::unique_ptr<Statement>> statements)
            : statements(std::move(statements)) {}
};

class ExpressionStatement : public Statement {
    public:
        std::unique_ptr<Expression> expression;
        ExpressionStatement(std::unique_ptr<Expression> expr)
            : expression(std::move(expr)) {}
};


class FunctionDefinition : public Function {
    public:
        std::string name;
        std::vector<std::string> parameters;
        std::string returnType;
        std::vector<std::unique_ptr<Statement>> body;
        FunctionDefinition(std::string name, std::vector<std::string> parameters, std::string returnType, std::vector<std::unique_ptr<Statement>> body)
            : name(std::move(name)), parameters(std::move(parameters)), returnType(std::move(returnType)), body(std::move(body)) {}
        std::string toString() const override {
            std::string params;
            for (const auto& param : parameters) {
                params += ",";
            }
            return "function " + name + "(" + params + ") -> " + returnType + " {}";
        }
};

class FunctionCall : public Function { //we only accept statements for function calls like: call add(3,2);
    public:
        std::string name;
        std::vector<std::unique_ptr<Expression>> arguments;
        FunctionCall(std::string name, std::vector<std::unique_ptr<Expression>> arguments)
            : name(std::move(name)), arguments(std::move(arguments)) {}
        std::string toString() const override {
            std::string args;
            for (const auto& arg : arguments) {
                args += ",";
            }
            return "call " + name + "(" + args + ")";
        }
};

//Program
class Program : public ASTNode {
    public:
        std::vector<std::unique_ptr<Declaration>> declarations;
        std::vector<std::unique_ptr<Statement>> statements;
        std::vector<std::unique_ptr<Function>> functions;
        std::vector<std::unique_ptr<Expression>> expressions;

        Program() = default;

        Program(std::vector<std::unique_ptr<Declaration>> declarations, std::vector<std::unique_ptr<Statement>> statements, std::vector<std::unique_ptr<Function>> functions, std::vector<std::unique_ptr<Expression>> expressions)
            : declarations(std::move(declarations)), statements(std::move(statements)), functions(std::move(functions)), expressions(std::move(expressions)) {}
};

#endif // ASTNODES_H