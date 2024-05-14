#ifndef ASTNODES_H
#define ASTNODES_H

#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <sstream>
#include <set>

#include "../symbolTable/SymbolTable.h"
#include "../visitor/Visitor.h"

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
        virtual std::string getName() const { return ""; }
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

class ArrayDeclaration : public Declaration {
public:
    std::string name;
	std::vector<std::unique_ptr<Expression>> elements;
    std::size_t size;

    ArrayDeclaration(std::string name, std::vector<std::unique_ptr<Expression>> elements)
        : name(std::move(name)), elements(std::move(elements)), size(this->elements.size()) {}

    std::string toString() const override {
		std::string result = "ArrayDeclaration(" + name + " = [";
        for (const auto& elem : elements) {
			result += elem->toString() + ", ";
		}
		result += "])";
		return result;
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

        std::string getName() const override {
			return name;
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

            //precompile regex
            static const std::regex intRegex("^[-+]?[0-9]+$");
            static const std::regex floatRegex("^[-+]?[0-9]*\\.[0-9]+$");
            static const std::regex stringRegex("^\".*\"$");

            if (std::regex_match(name, intRegex)) {
                std::cout << "Primary expression is an int for : " << name << std::endl;
                return "int";
            }
            // Check if the primary expression is a float
            else if (std::regex_match(name, floatRegex)) {
                return "float";
            }
            // Check if the primary expression is a string literal
            else if (std::regex_match(name, stringRegex)) {
                std::cout << "Primary expression is a string for : " << name << std::endl;
                return "string";
            }
            // Check if the primary expression is a boolean literal
            else if (name == "true" || name == "false") {
                return "bool";
            }
            // If it's not a recognized literal, assume it's an identifier and check if declared
            else {
                std::cout << "name of the primary expression is: " << name << "\n";
                auto symbolInfo = symbolTable.getSymbolInfo(name);
                if (!symbolInfo.has_value()) {
                   throw std::runtime_error("pE '" + name + "' not declared.");
                }
                std::cout << "Symbol type in primary expression is " << symbolInfo->type << " for: " << name << std::endl; // Print the type of the symbol
                return symbolInfo->type;
            }
        }

        std::string getName() const override {
            return name;
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

            std::cout << "Binary Expression: Left type: " << leftType << " Right type: " << rightType << "\n"; // Print the types of the left and right operands

            // List of operations that should return a boolean type
            std::set<std::string> comparisonOps = { "<", ">", "<=", ">=", "equals", "notEquals" };
            std::set<std::string> logicalOps = { "and", "or" };

            // Check if the operation is a comparison or logical operation
            if (comparisonOps.find(op) != comparisonOps.end() || logicalOps.find(op) != logicalOps.end()) {
                // For simplicity, assuming left and right operands are of compatible types for these operations
                std::cout << "We are in the comparison or logical operation\n";
                return "bool";
            }
            else if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
                // Arithmetic operations: return the type based on the operands
                if (leftType == "int" && rightType == "int") {
                    return "int";
                }
                else if (leftType == "float" && rightType == "float") {
                    return "float";
                }
                else {
                    throw std::runtime_error("Type mismatch in arithmetic binary expression. Unsupported operand types.");
                }
            }
            else {
                throw std::runtime_error("Unsupported binary operation for getType(): " + op);
            }
        }

        std::string getName() const override {
            return "binary";
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

        std::string getName() const override {
            return "unary";
        }

        void accept(IVisitor* visitor) const override {
            visitor->visit(this);
        }
};

class MethodCall : public Expression {
public:
    std::unique_ptr<Expression> object;
    std::string name;
	std::vector<std::unique_ptr<Expression>> arguments;

	MethodCall(std::unique_ptr<Expression> object, std::string name, std::vector<std::unique_ptr<Expression>> arguments)
		: object(std::move(object)), name(std::move(name)), arguments(std::move(arguments)) {}

    std::string toString() const override {
        std::string result = "MethodCall on " + object->toString() + " -> " + name + "(";
        for (const auto& arg : arguments) {
            if (&arg != &arguments.front()) result += ", ";
            result += arg->toString();
        }
        result += ")";
        return result;
    }

    std::string getName() const override {
		return name;
	}

    std::string getType(SymbolTable& symbolTable) const override {
		// Check if the method is declared
		auto symbolInfo = symbolTable.getSymbolInfo(name);
        if (!symbolInfo.has_value()) {
			throw std::runtime_error("Method " + name + " not declared.");
		}
		return symbolInfo->type;
	}

    void accept(IVisitor* visitor) const override {
		visitor->visit(this);
	}
        
};

//Statements

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
        std::unique_ptr<Statement> thenBody;
        std::unique_ptr<Statement> elseBody;
        
        IfStatement(std::unique_ptr<Expression> condition,
            std::unique_ptr<Statement> thenBody,
            std::unique_ptr<Statement> elseBody = nullptr) // elseBody is optional
            : condition(std::move(condition)), thenBody(std::move(thenBody)), elseBody(std::move(elseBody)) {}

        std::string toString() const override {
            return "IfStatement(" + condition->toString() + " then: " + thenBody->toString() +
                (elseBody ? " else: " + elseBody->toString() : "") + ")";
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

class FunctionCall : public Function {
    public:
        std::string name;
        std::vector<std::unique_ptr<Expression>> arguments;
        
        FunctionCall(std::string name, std::vector<std::unique_ptr<Expression>> arguments)
            : name(std::move(name)), arguments(std::move(arguments)) {}
        
        std::string toString() const override {
            std::string args;
            for (const auto& arg : arguments) {
                if (!args.empty()) args += ", "; 
                    args = args + "|" + arg->toString() + "|";
                }
            return "FunctionCall " + name + "(" + args + ")";
        }

        void accept(IVisitor* visitor) const override {
            std::cout << "Visiting function call in ASTNodes.h" << std::endl;
            visitor->visit(this);
        }
};
#endif // ASTNODES_H