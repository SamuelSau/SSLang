#ifndef ASTNODES_H
#define ASTNODES_H

#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <sstream>

#include "symbolTable/SymbolTable.h"
#include "visitor/Visitor.h"

class IVisitor;

class ASTNode {
    public:
        virtual ~ASTNode() = default;
        virtual std::string toString() const = 0;
        virtual void accept(IVisitor* visitor) const = 0;
};
class Declaration : public ASTNode {
    public:
};
class Expression : public ASTNode {
    public:
        virtual std::string getType(SymbolTable& symbolTable) const = 0;
};
class Statement : public ASTNode {
    public:
};
class Function : public ASTNode {
    public:
};
class Program : public ASTNode {
    public:
        std::vector<std::unique_ptr<Declaration>> declarations;
        std::vector<std::unique_ptr<Statement>> statements;
        std::vector<std::unique_ptr<Function>> functions;
        std::vector<std::unique_ptr<Expression>> expressions;

        Program() = default;

        Program(std::vector<std::unique_ptr<Declaration>> declarations, std::vector<std::unique_ptr<Statement>> statements, std::vector<std::unique_ptr<Function>> functions, std::vector<std::unique_ptr<Expression>> expressions)
            : declarations(std::move(declarations)), statements(std::move(statements)), functions(std::move(functions)), expressions(std::move(expressions)) {}

        std::string toString() const override {
            std::string result = "Program:\n";
            for (const auto& decl : declarations) {
                result += "Declaration: " + decl->toString() + "\n";
            }
            for (const auto& stmt : statements) {
                result += "Statement: " + stmt->toString() + "\n";
            }
            for (const auto& func : functions) {
                result += "Function: " + func->toString() + "\n";
            }
            for (const auto& expr : expressions) {
                result += "Expression: " + expr->toString() + "\n";
            }
            return result;
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

//Declarations
class IntDeclaration : public Declaration {
    public:
        std::string name;
        std::string number;

        IntDeclaration(std::string name, std::string number)
            : name(std::move(name)), number(std::move(number)) {}

        std::string toString() const override {
            return "IntDeclaration(" + name + " = " + number + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class FloatDeclaration : public Declaration {
    public:
        std::string name;
        std::string number;

        FloatDeclaration(std::string name, std::string number)
            : name(std::move(name)), number(std::move(number)) {}
        
        std::string toString() const override {
            return "FloatDeclaration(" + name + " = " + number + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class StringDeclaration : public Declaration {
    public:
        std::string name;
        std::string value;

        StringDeclaration(std::string name, std::string value)
            : name(std::move(name)), value(std::move(value)) {}
        
        std::string toString() const override {
            return "StringDeclaration(" + name + " = " + value + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class BoolDeclaration : public Declaration {
    public:
        std::string name;
        std::string value;

        BoolDeclaration(std::string name, std::string value)
            : name(std::move(name)), value(std::move(value)) {}
        
        std::string toString() const override {
            return "BoolDeclaration(" + name + " = " + value + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};


//Expressions
class AssignmentExpression : public Expression {
    public:
        std::string name;
        std::unique_ptr<Expression> expression;

        AssignmentExpression(std::string name, std::unique_ptr<Expression> expr)
            : name(std::move(name)), expression(std::move(expr)) {}

        std::string toString() const override {
            return "aE(" + name + " = " + expression->toString() + ";" + ")";
        }

        std::string getType(SymbolTable& symbolTable) const override {
        auto symbolInfo = symbolTable.getSymbolInfo(name);
        if (!symbolInfo.has_value()) {
            throw std::runtime_error("Variable " + name + " not declared.");
        }
        return symbolInfo->type;
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};


class LogicOrExpression : public Expression {
    public:
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;

        LogicOrExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
            : left(std::move(left)), right(std::move(right)) {}

        std::string toString() const override {
            return "loE(" + left->toString() + " " + right->toString() + ")";
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

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class LogicAndExpression : public Expression {
    public:
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;

        LogicAndExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
            : left(std::move(left)), right(std::move(right)) {}

        std::string toString() const override {
            return "laE(" + left->toString() + " " + right->toString() + ")";
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

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
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
            return "eE(" + left->toString() + " " + op + " " + right->toString() + ")";
        }

        std::string getType(SymbolTable& symbolTable) const override {
            auto leftType = left->getType(symbolTable);
            auto rightType = right->getType(symbolTable);

            // Simplified example: assume all operators work with integers only
            if ((leftType == "int" && rightType == "int") || (leftType == "float" && rightType == "float")) {
                return "bool";
            }            
            else {
                throw std::runtime_error("Type mismatch in equality expression. Only support int and float types.");
            }
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
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
            return "cE(" + left->toString() + " " + op + " " + right->toString() + ")";
        }

        std::string getType(SymbolTable& symbolTable) const override {
            auto leftType = left->getType(symbolTable);
            auto rightType = right->getType(symbolTable);

            // Simplified example: assume all operators work with integers only
            if ((leftType == "int" && rightType == "int") || (leftType == "float" && rightType == "float")) {
                return "bool"; // Comparison expressions result in a boolean
            } 
            else {
                throw std::runtime_error("Type mismatch in comparison expression. Both operands must be of the same type and either int or float.");
            }
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
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
            return "tE(" + left->toString() + " " + op + " " + right->toString() + ")";
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

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
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
            return "fE(" + left->toString() + " " + op + " " + right->toString() + ")";
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

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class PrimaryExpression : public Expression {
    public:
        std::string name;
        PrimaryExpression(std::string name)
            : name(std::move(name)) {}

        std::string toString() const override {
            return "pE(" + name + ")";
        }

        std::string getType(SymbolTable& symbolTable) const override {
            if (std::regex_match(name, std::regex("^[-+]?[0-9]+$"))) {
                return "int";
            }
            // Check if the primary expression is a float
            else if (std::regex_match(name, std::regex("^[-+]?[0-9]*\\.[0-9]+$"))) {
                return "float";
            }
            // Check if the primary expression is a string literal
            else if (std::regex_match(name, std::regex("^\".*\"$"))) {
                return "string";
            }
            // Check if the primary expression is a boolean literal
            else if (name == "true" || name == "false") {
                return "bool";
            }
            // If it's not a recognized literal, assume it's an identifier and check if declared
            else {
                auto symbolInfo = symbolTable.getSymbolInfo(name);
                if (!symbolInfo.has_value()) {
                    throw std::runtime_error("pE '" + name + "' not declared.");
                }
                //std::cout << "Symbol type in primary expression: " << symbolInfo->type << "\n"; // Print the type of the symbol
                return symbolInfo->type;
            }
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
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
            return "bE(" + left->toString() + " " + op + " " + right->toString() + ")";
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

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class UnaryExpression : public Expression {
    public:
        std::unique_ptr<Expression> expr;
        std::string op;

        UnaryExpression(std::unique_ptr<Expression> expr, std::string op)
            : expr(std::move(expr)), op(op) {}

        std::string toString() const override {
            return "uE(" + op + expr->toString() + ")";
        }

        std::string getType(SymbolTable& symbolTable) const override {
        std::string exprType = expr->getType(symbolTable);

        return exprType;
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

//Statements
class LoopStatement : public Statement {
    public:
        std::unique_ptr<Expression> start;
        std::unique_ptr<Expression> end;   
        std::unique_ptr<Statement> body;

        LoopStatement(std::unique_ptr<Expression> start, std::unique_ptr<Expression> end, std::unique_ptr<Statement> body)
            : start(std::move(start)), end(std::move(end)), body(std::move(body)) {}
        
        std::string toString() const override {
            return "LoopStatement(" + start->toString() + " " + end->toString() + " " + body->toString() + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class PrintStatement : public Statement {
    public:
        std::unique_ptr<Expression> expr;
        PrintStatement(std::unique_ptr<Expression> expr)
            : expr(std::move(expr)) {}
        
        std::string toString() const override {
            return "PrintStatement(" + expr->toString() + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class WhileLoopStatement : public Statement {
    public:
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Statement> body;
        WhileLoopStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
            : condition(std::move(condition)), body(std::move(body)) {}
        
        std::string toString() const override {
            return "WhileLoopStatement(" + condition->toString() + " " + body->toString() + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class ForLoopStatement : public Statement {
    public:
        std::unique_ptr<Expression> start;
        std::unique_ptr<Expression> end;
        std::unique_ptr<Statement> body;
        ForLoopStatement(std::unique_ptr<Expression> start, std::unique_ptr<Expression> end, std::unique_ptr<Statement> body)
            : start(std::move(start)), end(std::move(end)), body(std::move(body)) {}
        
        std::string toString() const override {
            return "ForLoopStatement(" + start->toString() + " " + end->toString() + " " + body->toString() + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class AssignmentStatement : public Statement {
    public:
        std::string name;
        std::unique_ptr<Expression> expression;
        AssignmentStatement(std::string name, std::unique_ptr<Expression> expr)
            : name(std::move(name)), expression(std::move(expr)) {}
        
        std::string toString() const override {
            return "AssignmentStatement(" + name + " = " + expression->toString() + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};


class IfStatement : public Statement {
    public:
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Statement> body;
        IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
            : condition(std::move(condition)), body(std::move(body)) {}
        
        std::string toString() const override {
            return "IfStatement(" + condition->toString() + " " + body->toString() + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class ElseStatement : public Statement {
    public:
        std::unique_ptr<Statement> body;
        ElseStatement(std::unique_ptr<Statement> body)
            : body(std::move(body)) {}
        
        std::string toString() const override {
            return "ElseStatement(" + body->toString() + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class ReturnStatement : public Statement {
    public:
        std::unique_ptr<Expression> expression;

        ReturnStatement(std::unique_ptr<Expression> expr)
            : expression(std::move(expr)) {}
        
        std::string toString() const override {
            return "ReturnStatement(" + expression->toString() + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class BlockStatement : public Statement {
    public:
        std::vector<std::unique_ptr<Statement>> statements;

        BlockStatement(std::vector<std::unique_ptr<Statement>> statements)
            : statements(std::move(statements)) {}
        
        std::string toString() const override {
            std::string result = "BlockStatement: ";
            for (const auto& stmt : statements) {
                result += stmt->toString() + " ";
            }
            return result;
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class ExpressionStatement : public Statement {
    public:
        std::unique_ptr<Expression> expression;
        ExpressionStatement(std::unique_ptr<Expression> expr)
            : expression(std::move(expr)) {}
        
        std::string toString() const override {
            return "ExpressionStatement(" + expression->toString() + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};


class FunctionDefinition : public Function {
    public:
        std::string name;
        std::vector<ParamInfo> parameters;
        std::string returnType;
        std::vector<std::unique_ptr<Statement>> body;
        
        FunctionDefinition(std::string name, std::vector<ParamInfo> parameters, std::string returnType, std::vector<std::unique_ptr<Statement>> body)
            : name(std::move(name)), parameters(std::move(parameters)), returnType(std::move(returnType)), body(std::move(body)) {}
        
        std::string toString() const override {
            std::stringstream ss;
            ss << "FunctionDefinition " << name << "(";
            for (size_t i = 0; i < parameters.size(); ++i) {
                ss << parameters[i].name << ": " << parameters[i].type;
                if (i < parameters.size() - 1) ss << ", ";
            }
            ss << ") -> " << returnType << " {\n";
            for (const auto& stmt : body) {
                ss << "\t" << stmt->toString() << "\n"; // Indent function body content for readability
            }
            ss << "}";

        return ss.str();
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
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
                if (!args.empty()) args += ", "; // Add a comma separator between arguments, but not before the first one
                    args += arg->toString(); // Call toString() on the pointed-to Expression
                }
            return "FunctionCall " + name + "(" + args + ")";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};
#endif // ASTNODES_H