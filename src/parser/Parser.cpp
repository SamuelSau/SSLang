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

    // Start with unary or primary expressions
    if (currentToken.is(Token::Kind::Identifier) && peekToken().is(Token::Kind::Equal)) {
            leftExp = parseAssignment(); 
    } 
    else if (currentToken.is(Token::Kind::Minus) || currentToken.is(Token::Kind::Not)) {
        // Definitely a unary operation
        leftExp = parseUnary();
    } 
    else if (currentToken.is(Token::Kind::Identifier) &&  (peekToken().is(Token::Kind::Plus) || peekToken().is(Token::Kind::Minus) || peekToken().is(Token::Kind::Asterisk) || peekToken().is(Token::Kind::Slash))) {
        leftExp = parseBinary();
    }
    else {
        // Default to primary for numbers, literals, etc.
        leftExp = parsePrimary();
    }
    return leftExp;
}


//Statement parsing

// std::unique_ptr<Statement> Parser::parseStatement() {
//     if (currentToken.is(Token::Kind::Identifier)) {
//         return parseAssignmentStatement();
//     }
//     else if (currentToken.is(Token::Kind::Log)) {
//         return parsePrintStatement();
//     }
//     else if (currentToken.is(Token::Kind::For)) {
//         return parseForStatement();
//     }
//     else if (currentToken.is(Token::Kind::If)) {
//         return parseIfStatement();
//     }
//     else if (currentToken.is(Token::Kind::While)) {
//         return parseWhileStatement();
//     }
//     else if (currentToken.is(Token::Kind::Return)) {
//         return parseReturnStatement();
//     }
//     else {
//         throw std::runtime_error("Expected statement type of either assignment, print, for, if, while, or return.");
//     }
// }

// std::unique_ptr<FunctionDefinition> Parser::parseFunctionDefinition() {
//     return nullptr; // Placeholder
// }

// std::unique_ptr<ASTNode> Parser::parseProgram() {
//     // Example implementation for parsing the program
//     std::vector<std::unique_ptr<Declaration>> declarations;
//     while (!atEnd()) {
//         declarations.push_back(parseDeclaration());
//     }
//     return std::make_unique<Program>(std::move(declarations)); // Assuming a Program node exists
// }

// std::unique_ptr<Expression> Parser::parseExpression() {
//     // Simple example: Assume expressions are just binary expressions or identifiers for now
//     auto left = parsePrimary();
    
//     while (currentToken.is(Token::Kind::Plus)) { // Simplified: handle '+' only for demonstration
//         char op = '+'; // Assuming currentToken.lexeme() would return "+"
//         consume(Token::Kind::Plus, "Expected '+'");
//         auto right = parsePrimary();
//         left = std::make_unique<BinaryExpression>(std::move(left), std::move(right), op);
//     }
    
//     return left;
// }

// std::unique_ptr<Expression> Parser::parsePrimary() {
//     if (currentToken.is(Token::Kind::Identifier)) {
//         auto expr = std::make_unique<Identifier>(std::string(currentToken.lexeme()));
//         consume(Token::Kind::Identifier, "Expected identifier.");
//         return expr;
//     // } else if (currentToken.is(Token::Kind::Number)) {
//     //     auto expr = std::make_unique<>(std::stoi(std::string(currentToken.lexeme())));
//     //     consume(Token::Kind::Number, "Expected number.");
//     //     return expr;
//     // } else if (currentToken.is(Token::Kind::LeftParen)) {
//     //     consume(Token::Kind::LeftParen, "Expected '('");
//     //     auto expr = parseExpression();
//     //     consume(Token::Kind::RightParen, "Expected ')'");
//     //     return expr;
//     // 
// }

//     throw std::runtime_error("Unexpected token in expression.");
// }


// std::unique_ptr<Statement> Parser::parseStatement() {
//     // Example: Distinguishing a block statement from others
//     if (currentToken.is(Token::Kind::LeftCurly)) {
//         return parseBlock();
//     }
//     // Fallback for now to treat all other statements as expression statements
//     auto expr = parseExpression();
//     consume(Token::Kind::Semicolon, "Expected ';' after expression statement.");
//     return std::make_unique<ExpressionStatement>(std::move(expr)); // Assuming ExpressionStatement exists
// }

// std::unique_ptr<BlockStatement> Parser::parseBlock() {
//     consume(Token::Kind::LeftCurly, "Expected '{' at the start of block.");
//     std::vector<std::unique_ptr<Statement>> statements;
//     while (!currentToken.is(Token::Kind::RightCurly) && !currentToken.is(Token::Kind::End)) {
//         statements.push_back(parseStatement());
//     }
//     consume(Token::Kind::RightCurly, "Expected '}' at the end of block.");
//     return std::make_unique<BlockStatement>(std::move(statements));
// }


// std::unique_ptr<Declaration> Parser::parseDeclaration() {
//     return nullptr; // Placeholder
// }


// std::unique_ptr<VariableDeclaration> Parser::parseVariableDeclaration() {
//     return nullptr; // Placeholder
// }

// std::unique_ptr<Expression> Parser::parseUnary() {
//     return nullptr; // Placeholder
// }

// std::unique_ptr<Expression> Parser::parseFactor() {
//     return nullptr; // Placeholder
// }

// std::unique_ptr<Expression> Parser::parseTerm() {
//     return nullptr; // Placeholder
// }

// std::unique_ptr<Expression> Parser::parseComparison() {
//     return nullptr; // Placeholder
// }

// std::unique_ptr<Expression> Parser::parseEquality() {
//     return nullptr; // Placeholder
// }

// std::unique_ptr<Expression> Parser::parseLogicAnd() {
//     return nullptr; // Placeholder
// }

// std::unique_ptr<Expression> Parser::parseLogicOr() {
//     return nullptr; // Placeholder
// }

// std::unique_ptr<Expression> Parser::parseAssignment() {
//     return nullptr; // Placeholder 
//}