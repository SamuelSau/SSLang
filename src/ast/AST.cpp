#include "ast/ASTNodes.h"
#include "parser/Parser.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>


std::unique_ptr<IntDeclaration> Parser::parseIntDeclaration() {
    consume(Token::Kind::Int, "Expected 'int' for this declaration.");

    if (!currentToken.is(Token::Kind::Identifier)) {
        throw std::runtime_error("Expected variable name after 'int'.");
    }
    std::string name = std::string(currentToken.lexeme());
    consume(Token::Kind::Identifier, "Expected identifier after 'int'.");

    consume(Token::Kind::Equal, "Expected '=' after variable name.");
    
    if (!currentToken.is(Token::Kind::Number)) {
        throw std::runtime_error("Expected number after '=' in variable declaration.");
    }
    std::string number = std::string(currentToken.lexeme());
    consume(Token::Kind::Number, "Expected integer after '='.");

    consume(Token::Kind::Semicolon, "Expected ';' after variable declaration.");
    
    return std::make_unique<IntDeclaration>(name, number);
}

std::unique_ptr<FloatDeclaration> Parser::parseFloatDeclaration() {
    consume(Token::Kind::Float, "Expected 'flt' for this declaration.");
    
    if (!currentToken.is(Token::Kind::Identifier)) {
        throw std::runtime_error("Expected variable name after 'flt'.");
    }

    std::string name = std::string(currentToken.lexeme());
    consume(Token::Kind::Identifier, "Expected identifier after 'float'.");

    consume(Token::Kind::Equal, "Expected '=' after variable name.");
    
    if (!currentToken.is(Token::Kind::FloatLiteral)) {
        throw std::runtime_error("Expected floating-point number after '=' in variable declaration.");
    }
    
    std::string number = std::string(currentToken.lexeme());
    consume(Token::Kind::FloatLiteral, "Expected floating-point number after '='."); // Corrected line

    consume(Token::Kind::Semicolon, "Expected ';' after variable declaration.");
    
    return std::make_unique<FloatDeclaration>(name, number);
}


std::unique_ptr<StringDeclaration> Parser::parseStringDeclaration(){
    consume(Token::Kind::String, "Expected 'str' for this declaration.");

    if (!currentToken.is(Token::Kind::Identifier)) {
        throw std::runtime_error("Expected variable name after 'str'.");
    }
    std::string name = std::string(currentToken.lexeme());
    consume(Token::Kind::Identifier, "Expected identifier after 'str'.");

    consume(Token::Kind::Equal, "Expected '=' after variable name.");
    
    if (!currentToken.is(Token::Kind::StringLiteral)) {
        throw std::runtime_error("Expected str after '=' in variable declaration.");
    }
    std::string value = std::string(currentToken.lexeme());
    consume(Token::Kind::StringLiteral, "Expected a string literal after '='.");

    consume(Token::Kind::Semicolon, "Expected ';' after variable declaration.");
    
    std::cout << "Parsed string declaration: " << name << " = " << value << std::endl;

    return std::make_unique<StringDeclaration>(name, value);

}

// std::unique_ptr<FunctionDefinition> Parser::parseFunctionDefinition() {
//     consume(Token::Kind::Function, "Expected 'function' keyword.");
//     consume(Token::Kind::Identifier, "Expected function name.");
//     std::string name = std::string(currentToken.lexeme());

//     // Parameter parsing
//     std::vector<std::pair<std::string, std::string>> parameters;
//     consume(Token::Kind::LeftParen, "Expected '(' after function name.");
//     while (!currentToken.is(Token::Kind::RightParen)) {
//         if (!currentToken.is(Token::Kind::Identifier)) {
//             throw std::runtime_error("Expected parameter name.");
//         }
//         std::string paramName = std::string(currentToken.lexeme());
//         consume(Token::Kind::Identifier, "Expected parameter name.");

//         consume(Token::Kind::Colon, "Expected ':' after parameter name.");
//         if (!currentToken.is(Token::Kind::Identifier)) {
//             throw std::runtime_error("Expected parameter type.");
//         }
//         std::string paramType = std::string(currentToken.lexeme());
//         consume(Token::Kind::Identifier, "Expected parameter type.");

//         parameters.emplace_back(paramName, paramType);

//         if (currentToken.is(Token::Kind::Comma)) {
//             consume(Token::Kind::Comma, "Expected ',' between parameters.");
//         } else if (!currentToken.is(Token::Kind::RightParen)) {
//             throw std::runtime_error("Expected ',' or ')' after parameter.");
//         }
//     }
//     consume(Token::Kind::RightParen, "Expected ')' after parameters.");

//     consume(Token::Kind::Arrow, "Expected '->' after parameters.");
//     consume(Token::Kind::Identifier, "Expected return type.");
//     std::string returnType = std::string(currentToken.lexeme());

//     // Function body parsing
//     std::vector<std::unique_ptr<Statement>> body;
//     consume(Token::Kind::LeftCurly, "Expected '{' before function body.");
//     while (!currentToken.is(Token::Kind::RightCurly) && !currentToken.is(Token::Kind::End)) {
//         body.push_back(parseStatement()); // Parse each statement in the body
//     }
//     consume(Token::Kind::RightCurly, "Expected '}' after function body.");

//     return std::make_unique<FunctionDefinition>(name, parameters, returnType, std::move(body));
// }
