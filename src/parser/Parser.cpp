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
    nextToken = lexer.next(); 
    
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

    else if (currentToken.is(Token::Kind::Bool)) {
        return parseBoolDeclaration(); 
    }
    else {
        throw std::runtime_error("Expected declaration type of either int, flt, or str.");
    }
}

std::unique_ptr<Expression> Parser::parseExpression() {
    std::unique_ptr<Expression> leftExp;
    if (currentToken.is(Token::Kind::End)){
        throw std::runtime_error("Unexpected end of file. Expected expression.");
    }
    else if (currentToken.is_one_of(Token::Kind::Int, Token::Kind::Float, Token::Kind::String, Token::Kind::Bool)) {
        throw std::runtime_error("Forbidden keyword for expressions! Should not be able to parse as expression! Please use \"int\", \"flt\", \"str\", \"bool\" for declarations.");
    }
    else if (currentToken.is(Token::Kind::Return)){
        throw std::runtime_error("Not allowed to return without it being in a function definition");
    }
    else if (currentToken.is_one_of(Token::Kind::While, Token::Kind::For)){
        throw std::runtime_error("Forbidden keyword for expressions! Please use \"loop\" for while and for.");
    }
    else if (currentToken.is_one_of(Token::Kind::Function, Token::Kind::Call)){
        throw std::runtime_error("No function definitions or calls allowed in expressions. Only reserved for statements.");
    }
    else if (currentToken.is(Token::Kind::Comment)){
        advance();
    }
    // Start with unary or primary expressions
    else if (currentToken.is(Token::Kind::Identifier) && peekToken().is(Token::Kind::Equal)) {
        leftExp = parseAssignment(); 
    } 
    else if (currentToken.is(Token::Kind::Minus) || currentToken.is(Token::Kind::Not)) {
        // Definitely a unary operation
        leftExp = parseUnary();
    } 
    else if (currentToken.is_one_of(Token::Kind::Identifier, Token::Kind::Number, Token::Kind::FloatLiteral) &&  (peekToken().is(Token::Kind::Plus) || peekToken().is(Token::Kind::Minus) || peekToken().is(Token::Kind::Asterisk) || peekToken().is(Token::Kind::Slash))) {
        leftExp = parseBinary();
    }
    else if (currentToken.is_one_of(Token::Kind::Identifier, Token::Kind::Number, Token::Kind::FloatLiteral) && (peekToken().is(Token::Kind::LessThan) || peekToken().is(Token::Kind::GreaterThan) || peekToken().is(Token::Kind::Equals) || peekToken().is(Token::Kind::NotEquals))) {
        leftExp = parseComparison();
    }
    else {
        
        if (currentToken.is(Token::Kind::End)){
            return leftExp;
        }
        
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
    else if (currentToken.is(Token::Kind::Else)){
        return parseElseStatement();
    }
    else if (currentToken.is(Token::Kind::Return)) {
        return parseReturnStatement();
    }
    else {
        if (currentToken.is_one_of(Token::Kind::For, Token::Kind::While)) {
            throw std::runtime_error("Forbidden keyword for loop statements. Please use \"loop\" for while and for.");
        }
        else if (currentToken.is(Token::Kind::Print)) {
            throw std::runtime_error("Forbidden keyword for printing statements. Please use \"log()\"");
        }
        else if (currentToken.is_one_of(Token::Kind::Int, Token::Kind::String, Token::Kind::Float, Token::Kind::Bool)) {
            throw std::runtime_error("Forbidden keyword for statements! Should not be able to parse as statement! Please use \"int\", \"flt\", \"str\", \"bool\" for declarations.");
        }
        else if (currentToken.is(Token::Kind::Comment)){
           advance();
        }
        else {
            auto expr = parseExpression();
            return std::make_unique<ExpressionStatement>(std::move(expr));
        }
        auto expr = parseExpression();
        return std::make_unique<ExpressionStatement>(std::move(expr));
    }
}

std::unique_ptr<Function> Parser::parseFunction(){
    if (currentToken.is(Token::Kind::Function)) {
        return parseFunctionDefinition();
    }
    else if (currentToken.is(Token::Kind::Call)){
        return parseFunctionCall();
    }
    else {
        throw std::runtime_error("Expected valid function call or definition.");
    }
}
