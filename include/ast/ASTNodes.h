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
};
class Statement : public ASTNode {
    public:
};
class Declaration : public ASTNode {
    public:
};

class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Declaration>> declarations;
    Program(std::vector<std::unique_ptr<Declaration>> declarations)
        : declarations(std::move(declarations)) {}
};
 
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

class FunctionDefinition : public Declaration {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters;
    std::string returnType;
    std::vector<std::unique_ptr<Statement>> body;
    FunctionDefinition(std::string name, std::vector<std::pair<std::string, std::string>> parameters, std::string returnType, std::vector<std::unique_ptr<Statement>> body)
        : name(std::move(name)), parameters(std::move(parameters)), returnType(std::move(returnType)), body(std::move(body)) {}
};

class BinaryExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    char op;
    BinaryExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, char op)
        : left(std::move(left)), right(std::move(right)), op(op) {}
};

class UnaryExpression : public Expression {
public:
    std::unique_ptr<Expression> expr;
    char op;
    UnaryExpression(std::unique_ptr<Expression> expr, char op)
        : expr(std::move(expr)), op(op) {}
};

class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    virtual ~ExpressionStatement() = default;
    
    ExpressionStatement(std::unique_ptr<Expression> expr)
        : expression(std::move(expr)) {}
};

class Literal : public Expression {
public:
    std::string value;
    Literal(std::string value) : value(std::move(value)) {}
};

class Identifier : public Expression {
public:
    std::string name;
    Identifier(std::string name) : name(std::move(name)) {}
};

#endif // ASTNODES_H