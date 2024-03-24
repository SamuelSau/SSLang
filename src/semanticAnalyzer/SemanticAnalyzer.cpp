
#include "semanticAnalyzer/SemanticAnalyzer.h"

SemanticAnalyzer::SemanticAnalyzer(SymbolTable& symbolTable) : symbolTable(symbolTable) {}

//Implementation of the visit methods
void SemanticAnalyzer::visit(const IntDeclaration* decl) {
}

void SemanticAnalyzer::visit(const FloatDeclaration* decl) {
}

void SemanticAnalyzer::visit(const StringDeclaration* decl) {
}

void SemanticAnalyzer::visit(const AssignmentExpression* expr) {
}

void SemanticAnalyzer::visit(const LogicOrExpression* expr) {
}

void SemanticAnalyzer::visit(const LogicAndExpression* expr) {
}

void SemanticAnalyzer::visit(const EqualityExpression* expr) {
}

void SemanticAnalyzer::visit(const ComparisonExpression* expr) {
}

void SemanticAnalyzer::visit(const TermExpression* expr) {
}

void SemanticAnalyzer::visit(const FactorExpression* expr) {
}

 void SemanticAnalyzer::visit(const BinaryExpression* expr) {
 }

void SemanticAnalyzer::visit(const PrimaryExpression* expr) {

}

void SemanticAnalyzer::visit(const UnaryExpression* expr) {

}

void SemanticAnalyzer::visit(const LoopStatement* stmt) {

}

void SemanticAnalyzer::visit(const PrintStatement* stmt) {
}

void SemanticAnalyzer::visit(const WhileLoopStatement* stmt) {
}

void SemanticAnalyzer::visit(const ForLoopStatement* stmt) {
}

void SemanticAnalyzer::visit(const AssignmentStatement* stmt) {
}

void SemanticAnalyzer::visit(const IfStatement* stmt) {
}

void SemanticAnalyzer::visit(const ElseStatement* stmt) {
}

void SemanticAnalyzer::visit(const ReturnStatement* stmt) {
}

void SemanticAnalyzer::visit(const BlockStatement* stmt) {
}

void SemanticAnalyzer::visit(const ExpressionStatement* stmt) {
}

void SemanticAnalyzer::visit(const FunctionDefinition* decl) {
}

void SemanticAnalyzer::visit(const FunctionCall* call) {
}

void SemanticAnalyzer::visit(const Program* program) {
    // Optionally, traverse the entire program structure
}