#include <stdexcept>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "parser/Parser.h"


bool Parser::atEnd() const {
    return currentToken.is(Token::Kind::End);
}

void Parser::advance() {
   if (nextToken.kind() == Token::Kind::Uninitialized) { // Assume such a kind exists for demonstration
        nextToken = lexer.next(); // Ensure nextToken has initial value
    }
    currentToken = std::move(nextToken); // Safely move now that nextToken is guaranteed to have been loaded
    nextToken = lexer.next(); // Preload the next token
}

void Parser::consume(Token::Kind kind, const std::string& errorMessage) {
    if (currentToken.is(kind)) {
        advance();
    }
    else {
        throw std::runtime_error(errorMessage);
    
    }
}

const Token& Parser::peekToken() const {
    return nextToken;
}

//Declaration parsing
std::unique_ptr<Declaration> Parser::parseDeclaration() {
    if (currentToken.is(Token::Kind::Int)) {
        return parseIntDeclaration();
    }
    else if (currentToken.is(Token::Kind::Float)) {
        return parseFloatDeclaration();
    }
    else if (currentToken.is(Token::Kind::String)) {
        return parseStringDeclaration();
    } 
     else {
        throw std::runtime_error("Expected declaration type of either int, float, or string.");
    }
}

std::unique_ptr<Expression> Parser::parseExpression() {
    std::unique_ptr<Expression> leftExp;
    if (currentToken.is(Token::Kind::End)){
        throw std::runtime_error("Unexpected end of file. Expected expression.");
    }
    if (currentToken.is(Token::Kind::LeftParen)){
        consume(Token::Kind::LeftParen, "Expected '(' at the beginning of an expression.");
    }
    // Start with unary or primary expressions
    if (currentToken.is(Token::Kind::Identifier) && peekToken().is(Token::Kind::Equal)) {
            leftExp = parseAssignment(); 
    } 
    else if (currentToken.is(Token::Kind::Minus) || currentToken.is(Token::Kind::Not)) {
        // Definitely a unary operation
        leftExp = parseUnary();
    } 
    else if (currentToken.is_one_of(Token::Kind::Identifier, Token::Kind::Number, Token::Kind::FloatLiteral) &&  (peekToken().is(Token::Kind::Plus) || peekToken().is(Token::Kind::Minus) || peekToken().is(Token::Kind::Asterisk) || peekToken().is(Token::Kind::Slash))) {
        leftExp = parseBinary();
    }
    else if (currentToken.is_one_of(Token::Kind::Identifier, Token::Kind::Number, Token::Kind::FloatLiteral) && peekToken().is(Token::Kind::LessThan) || peekToken().is(Token::Kind::GreaterThan) || peekToken().is(Token::Kind::Equal) || peekToken().is(Token::Kind::NotEquals)) {
        leftExp = parseComparison();
    }
    else {
        // Default to primary for numbers, literals, etc.
        leftExp = parsePrimary();
    }
    return leftExp;
}

//Statement parsing
std::unique_ptr<Statement> Parser::parseStatement() {
    if (currentToken.is(Token::Kind::Loop)) {
        return parseLoopStatement();
    }
    else if (currentToken.is(Token::Kind::Log)) {
        return parsePrintStatement();
    }
    else if (currentToken.is(Token::Kind::If)) {
        return parseIfStatement();
    }
    else if (currentToken.is(Token::Kind::Return)) {
        return parseReturnStatement();
    }
    else {

        if (currentToken.is_one_of(Token::Kind::For, Token::Kind::While)) {
            throw std::runtime_error("Forbidden keyword for statements. Please use loop.");
        }
        else if (currentToken.is(Token::Kind::Print)) {
            throw std::runtime_error("Forbidden keyword for statements. Please use log().");
        }

        if (peekToken().is(Token::Kind::LeftParen)) {
            while (!currentToken.is(Token::Kind::Semicolon)) {
                advance();
                if (currentToken.is(Token::Kind::LeftCurly)) {
                    throw std::runtime_error("Wrong keyword used in statement. Only supported with log, loop, if, and return.");
                }
            }
        }

        auto expr = parseExpression();
        return std::make_unique<ExpressionStatement>(std::move(expr));
    }
}

std::unique_ptr<Function> Parser::parseFunction(){
    if (currentToken.is(Token::Kind::Function)) {
        return parseFunctionDefinition();
    }
    else if (currentToken.is(Token::Kind::Call)) {
        return parseFunctionCall();
    }
    else {
        throw std::runtime_error("Expected valid function call or definition.");
    }
}