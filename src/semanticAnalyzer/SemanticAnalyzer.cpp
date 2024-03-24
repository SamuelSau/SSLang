
#include "semanticAnalyzer/SemanticAnalyzer.h"

SemanticAnalyzer::SemanticAnalyzer(SymbolTable& symbolTable) : symbolTable(symbolTable) {}

std::string SemanticAnalyzer::evaluateExpressionType(const Expression* expr) {
    // Handle different types of expressions using dynamic_cast to identify the type of expression
    if (const auto* binaryExpr = dynamic_cast<const BinaryExpression*>(expr)) {
        return evaluateBinaryExpressionType(binaryExpr);
    } else if (const auto* primaryExpr = dynamic_cast<const PrimaryExpression*>(expr)) {
        return evaluatePrimaryExpressionType(primaryExpr);
    } else if (const auto* unaryExpr = dynamic_cast<const UnaryExpression*>(expr)) {
        return evaluateUnaryExpressionType(unaryExpr);
    }
    throw std::runtime_error("Unknown expression type.");
}

void SemanticAnalyzer::checkAssignment(const std::string& varName, const Expression* expr) {
    auto varInfo = symbolTable.getSymbolInfo(varName);
    auto exprType = evaluateExpressionType(expr);
    if (!varInfo || varInfo->type != exprType) {
        throw std::runtime_error("Type mismatch in assignment to " + varName);
    }
}

void SemanticAnalyzer::checkFunctionCall(const FunctionCall* call) {
    auto funcInfo = symbolTable.getFunctionInfo(call->name);
    if (!funcInfo.has_value()) {
        throw std::runtime_error("Function " + call->name + " not declared.");
    }
    if (call->arguments.size() != funcInfo->parameterTypes.size()) {
        throw std::runtime_error("Incorrect number of arguments in call to " + call->name);
    }

    for (size_t i = 0; i < call->arguments.size(); ++i) {
        auto argType = evaluateExpressionType(call->arguments[i].get()); // Assuming arguments is a vector of unique_ptr<Expression>
        if (argType != funcInfo->parameterTypes[i]) {
            throw std::runtime_error("Type mismatch in argument " + std::to_string(i + 1) + " in call to " + call->name);
        }
    }
}

std::string SemanticAnalyzer::evaluateBinaryExpressionType(const BinaryExpression* expr) {
    auto leftType = evaluateExpressionType(expr->left.get());
    auto rightType = evaluateExpressionType(expr->right.get());
    if (leftType == rightType) {
        return leftType;
    }
    throw std::runtime_error("Type mismatch in binary expression");
}

std::string SemanticAnalyzer::evaluatePrimaryExpressionType(const PrimaryExpression* expr) {
    try {
        return expr->getType(symbolTable);
    } catch (const std::runtime_error& e) {
        throw e.what();
    }
}

std::string SemanticAnalyzer::evaluateUnaryExpressionType(const UnaryExpression* expr) {
    try {
        return expr->getType(symbolTable);
    } catch (const std::runtime_error& e) {
        throw e.what();
    }
}

