#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "../symbolTable/SymbolTable.h"
#include "../visitor/Visitor.h"


class SemanticAnalyzer : public IVisitor{
public:
    explicit SemanticAnalyzer(SymbolTable& symbolTable);

    void visit(const IntDeclaration* decl);
    void visit(const FloatDeclaration* decl);
    void visit(const StringDeclaration* decl);
    void visit(const BoolDeclaration* decl);
    void visit(const ArrayDeclaration* decl);

    void visit(const BinaryExpression* expr);
    void visit(const PrimaryExpression* expr);
    void visit(const UnaryExpression* expr);
    void visit(const AssignmentExpression* expr);
    void visit(const MethodCall* expr);

    void visit(const PrintStatement* stmt);
    void visit(const WhileLoopStatement* stmt);
    void visit(const ForLoopStatement* stmt);
    void visit(const AssignmentStatement* stmt);
    void visit(const IfStatement* stmt);
    void visit(const ReturnStatement* stmt);
    void visit(const BlockStatement* stmt);
    void visit(const ExpressionStatement* stmt);

    void visit(const FunctionDefinition* decl);
    void visit(const FunctionCall* call);

    void visit(const Program* program);
    
private:
    SymbolTable& symbolTable;
    std::string currentFunctionReturnType;
    bool insideFunction = false;
};

#endif // SEMANTIC_ANALYZER_H
