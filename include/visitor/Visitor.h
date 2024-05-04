#ifndef VISITOR_H
#define VISITOR_H

class Expression;
class IntDeclaration;
class FloatDeclaration;
class BoolDeclaration;
class StringDeclaration;
class ArrayDeclaration;
class AssignmentExpression;
class LogicOrExpression;
class LogicAndExpression;
class EqualityExpression;
class ComparisonExpression;
class TermExpression;
class FactorExpression;
class PrimaryExpression;
class BinaryExpression;
class UnaryExpression;
class MethodCall;
class LoopStatement;
class PrintStatement;
class WhileLoopStatement;
class ForLoopStatement;
class AssignmentStatement;
class IfStatement;
class ReturnStatement;
class BlockStatement;
class ExpressionStatement;
class FunctionDefinition;
class FunctionCall;
class Program;

class IVisitor {
public:
    virtual void visit(const IntDeclaration* decl) = 0;
    virtual void visit(const FloatDeclaration* decl) = 0;
    virtual void visit(const StringDeclaration* decl) = 0;
    virtual void visit(const BoolDeclaration* decl) = 0;
    virtual void visit(const ArrayDeclaration* decl) = 0;

    virtual void visit(const AssignmentExpression* expr) = 0;
    virtual void visit(const PrimaryExpression* expr) = 0;
    virtual void visit(const BinaryExpression* expr) = 0;   
    virtual void visit(const UnaryExpression* expr) = 0;
    virtual void visit(const MethodCall* expr) = 0;
    
    virtual void visit(const PrintStatement* stmt) = 0;
    virtual void visit(const WhileLoopStatement* stmt) = 0;
    virtual void visit(const ForLoopStatement* stmt) = 0;
    virtual void visit(const AssignmentStatement* stmt) = 0;
    virtual void visit(const IfStatement* stmt) = 0;
    virtual void visit(const ReturnStatement* stmt) = 0;
    virtual void visit(const BlockStatement* stmt) = 0;
    virtual void visit(const ExpressionStatement* decl) = 0;

    virtual void visit(const FunctionDefinition* decl) = 0;
    virtual void visit(const FunctionCall* call) = 0;
    virtual void visit(const Program* program) = 0;

    virtual ~IVisitor() = default;
};


#endif // VISITOR_H
