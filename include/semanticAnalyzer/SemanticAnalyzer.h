#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "symbolTable/SymbolTable.h"
#include "visitor/Visitor.h"

class SemanticAnalyzer {
public:
    explicit SemanticAnalyzer(SymbolTable& symbolTable);

    void visit(const IntDeclaration* decl);
    void visit(const FloatDeclaration* decl);
    void visit(const StringDeclaration* decl);

    void visit(const BinaryExpression* expr);
    void visit(const PrimaryExpression* expr);
    void visit(const UnaryExpression* expr);
    void visit(const AssignmentExpression* expr);
    void visit(const LogicOrExpression* expr);
    void visit(const LogicAndExpression* expr);
    void visit(const EqualityExpression* expr);
    void visit(const ComparisonExpression* expr);
    void visit(const TermExpression* expr);
    void visit(const FactorExpression* expr);

    void visit(const LoopStatement* stmt);
    void visit(const PrintStatement* stmt);
    void visit(const WhileLoopStatement* stmt);
    void visit(const ForLoopStatement* stmt);
    void visit(const AssignmentStatement* stmt);
    void visit(const IfStatement* stmt);
    void visit(const ElseStatement* stmt);
    void visit(const ReturnStatement* stmt);
    void visit(const BlockStatement* stmt);
    void visit(const ExpressionStatement* stmt);

    void visit(const FunctionDefinition* decl);
    void visit(const FunctionCall* call);

    void visit(const Program* program);
    

private:
    SymbolTable& symbolTable;
};

#endif // SEMANTIC_ANALYZER_H
