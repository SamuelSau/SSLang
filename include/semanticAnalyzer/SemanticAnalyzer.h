// SemanticAnalyzer.h
#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "ast/ASTNodes.h"
#include "symbolTable/SymbolTable.h"

class SemanticAnalyzer {
public:
    explicit SemanticAnalyzer(SymbolTable& symbolTable);

    std::string evaluateExpressionType(const Expression* expr);
    void checkAssignment(const std::string& varName, const Expression* expr);
    void checkFunctionCall(const FunctionCall* call);

private:
    SymbolTable& symbolTable;

    // Helper methods for specific expression type checks
    std::string evaluateBinaryExpressionType(const BinaryExpression* expr);
    std::string evaluatePrimaryExpressionType(const PrimaryExpression* expr);
    std::string evaluateUnaryExpressionType(const UnaryExpression* expr);
    
};

#endif // SEMANTIC_ANALYZER_H
