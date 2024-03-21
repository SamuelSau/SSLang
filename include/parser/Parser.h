// Parser.h
#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>
#include <optional> 
#include <iostream>


#include "lexer/Lexer.h"
#include "ast/ASTNodes.h"

class Parser {
public:
    explicit Parser(Lexer& lexer) : lexer(lexer) {
        advance();
    }
    Lexer& lexer;
    Token currentToken;
    bool atEnd() const;
    void advance();
    void consume(Token::Kind kind, const std::string& errorMessage);

    //Declaration parsing
    std::unique_ptr<Declaration> parseDeclaration(); 
    std::unique_ptr<IntDeclaration> parseIntDeclaration(); //int x = 5;
    std::unique_ptr<FloatDeclaration> parseFloatDeclaration(); //flt x = 5.0;
    std::unique_ptr<StringDeclaration> parseStringDeclaration(); //str x = "hello";

    //Function parsing
    //std::unique_ptr<FunctionDefinition> parseFunctionDefinition(); //function add(a: int, b: int) -> int {}

    //Statement parsing
    // std::unique_ptr<Statement> parseStatement();
    // std::unique_ptr<PrintStatement> parsePrintStatement(); //log(2+3)
    // std::unique_ptr<AssignmentStatement> parseAssignmentStatement();
    // std::unique_ptr<ForStatement> parseForStatement(); //for range(2,3) {}
    // std::unique_ptr<IfStatement> parseIfStatement();
    // std::unique_ptr<WhileStatement> parseWhileStatement();
    // std::unique_ptr<ReturnStatement> parseReturnStatement(); //return 2+3;

    // // Expression parsing
    // std::unique_ptr<Expression> parseExpression();
    // std::unique_ptr<AssignmentExpression> parseAssignment(); //x = 5 (where x is already declared)
    // std::unique_ptr<LogicOrExpression> parseLogicOr(); //x or y
    // std::unique_ptr<LogicAndExpression> parseLogicAnd(); //x and y
    // std::unique_ptr<EqualityExpression> parseEquality(); //x equals 5
    // std::unique_ptr<ComparisonExpression> parseComparison(); //x > 5
    // std::unique_ptr<TermExpression> parseTerm(); //x + 5
    // std::unique_ptr<FactorExpression> parseFactor(); //x * 5
    // std::unique_ptr<UnaryExpression> parseUnary(); //-x, not x
    // std::unique_ptr<PrimaryExpression> parsePrimary(); //x
};

#endif // PARSER_H