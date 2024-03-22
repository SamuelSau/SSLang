#ifndef ASTNODES_H
#define ASTNODES_H

#include <string>
#include <vector>
#include <memory>

class ASTNode {
    public:
        virtual ~ASTNode() = default;
};

class Expression : public ASTNode {
    public:
        virtual std::string toString() const = 0;  //pure virtual function
};
class Statement : public ASTNode {
    public:
};
class Declaration : public ASTNode {
    public:
};

class Definition : public ASTNode {
    public:
};

class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Declaration>> declarations;
    Program(std::vector<std::unique_ptr<Declaration>> declarations)
        : declarations(std::move(declarations)) {}
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

//Statements
class PrintStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    PrintStatement(std::unique_ptr<Expression> expr)
        : expression(std::move(expr)) {}
};

class AssignmentStatement : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> expression;
    AssignmentStatement(std::string name, std::unique_ptr<Expression> expr)
        : name(std::move(name)), expression(std::move(expr)) {}
};

class ForStatement : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> start;
    std::unique_ptr<Expression> end;
    std::vector<std::unique_ptr<Statement>> body;
    ForStatement(std::string name, std::unique_ptr<Expression> start, std::unique_ptr<Expression> end, std::vector<std::unique_ptr<Statement>> body)
        : name(std::move(name)), start(std::move(start)), end(std::move(end)), body(std::move(body)) {}
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> body;
    IfStatement(std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Statement>> body)
        : condition(std::move(condition)), body(std::move(body)) {}
};

class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> body;
    WhileStatement(std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Statement>> body)
        : condition(std::move(condition)), body(std::move(body)) {}
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    ReturnStatement(std::unique_ptr<Expression> expr)
        : expression(std::move(expr)) {}
};

//Definitions
class FunctionDefinition : public Definition {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters;
    std::string returnType;
    std::vector<std::unique_ptr<Statement>> body;
    FunctionDefinition(std::string name, std::vector<std::pair<std::string, std::string>> parameters, std::string returnType, std::vector<std::unique_ptr<Statement>> body)
        : name(std::move(name)), parameters(std::move(parameters)), returnType(std::move(returnType)), body(std::move(body)) {}
};

//Expressions

class AssignmentExpression : public Expression {
public:
    std::string name;
    std::unique_ptr<Expression> expression;
    AssignmentExpression(std::string name, std::unique_ptr<Expression> expr)
        : name(std::move(name)), expression(std::move(expr)) {}
    std::string toString() const override {
        return "(" + name + " = " + expression->toString() + ";" + ")";
    }
};

class LogicOrExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    LogicOrExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : left(std::move(left)), right(std::move(right)) {}
};

class LogicAndExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    LogicAndExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : left(std::move(left)), right(std::move(right)) {}
};

class EqualityExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    std::string op;
    EqualityExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, std::string op)
        : left(std::move(left)), right(std::move(right)), op(op) {}
};

class ComparisonExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    std::string op;
    ComparisonExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, std::string op)
        : left(std::move(left)), right(std::move(right)), op(op) {}
};

class TermExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    std::string op;
    TermExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, std::string op)
        : left(std::move(left)), right(std::move(right)), op(op) {}
    std::string toString() const override {
        return "(" + left->toString() + " " + op + " " + right->toString() + ")";
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
        return "(" + left->toString() + " " + op + " " + right->toString() + ")";
    }
};

class PrimaryExpression : public Expression {
public:
    std::string name;
    PrimaryExpression(std::string name)
        : name(std::move(name)) {}
    std::string toString() const override {
        return name;
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
        return "(" + left->toString() + " " + op + " " + right->toString() + ")";
    }
};

class UnaryExpression : public Expression {
public:
    std::unique_ptr<Expression> expr;
    std::string op;
    UnaryExpression(std::unique_ptr<Expression> expr, std::string op)
        : expr(std::move(expr)), op(op) {}
    std::string toString() const override {
        return "(" + op + expr->toString() + ")";
    }
};


#endif // ASTNODES_H