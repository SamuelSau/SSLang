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
//Parsing expressions
std::unique_ptr<Expression> Parser::parseAssignment() {
    // Example: Parsing "x = 5;"
    if (currentToken.is(Token::Kind::Identifier)) {
        std::string variableName = std::string(currentToken.lexeme()); // Capture the variable name
        consume(Token::Kind::Identifier, "Expected an identifier"); // Move past the identifier

        if (currentToken.is(Token::Kind::Equal)) {
            consume(Token::Kind::Equal, "Expected an = after identifier"); // Consume the '=' operator
            auto right = parseExpression(); // Recursively parse the right-hand side expression
            consume(Token::Kind::Semicolon, "Expected ';' after expression");

            // Create an AssignmentExpression with the variable name and the expression
            return std::make_unique<AssignmentExpression>(std::move(variableName), std::move(right));
        } else {
            // Error handling for missing '=' in an assignment
            throw std::runtime_error("Expected '=' in assignment");
        }
    }
    if (currentToken.is(Token::Kind::Number) || currentToken.is(Token::Kind::FloatLiteral) || currentToken.is(Token::Kind::StringLiteral)) {
        return parsePrimary();
    }
    // If the left side isn't an identifier, handle according to your language's syntax rules
    // This might be an error, or you might have other forms of expressions that are valid here
    throw std::runtime_error("Expected an identifier in assignment");
}

std::unique_ptr<Expression> Parser::parseLogicOr() {
    auto left = parseLogicAnd(); // Start with higher precedence expressions
    while (currentToken.is(Token::Kind::Or)) {
        std::string op = std::string(currentToken.lexeme());
        consume(Token::Kind::Or, "Expected 'or'");
        auto right = parseLogicAnd(); // Parse the right operand
        left = std::make_unique<LogicOrExpression>(std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseLogicAnd() {
    auto left = parseEquality(); // Proceed to the next precedence level
    while (currentToken.is(Token::Kind::And)) {
        std::string op = std::string(currentToken.lexeme());
        consume(Token::Kind::And, "Expected 'and'");
        auto right = parseEquality(); // Parse the right operand
        left = std::make_unique<LogicAndExpression>(std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseEquality() {
    auto left = parseComparison(); // Proceed to the next precedence level
    while (currentToken.is(Token::Kind::Equals)) {
        std::string op = std::string(currentToken.lexeme());
        consume(Token::Kind::Equals, "Expected 'equals'");
        auto right = parseComparison(); // Parse the right operand
        left = std::make_unique<EqualityExpression>(std::move(left), std::move(right), op);
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseComparison() {
    auto left = parseTerm(); // This assumes parseTerm handles addition/subtraction
    while (currentToken.is_one_of(Token::Kind::GreaterThan, Token::Kind::LessThan)) {
        std::string op = std::string(currentToken.lexeme());
        consume(currentToken.kind(), "Expected comparison operator");
        auto right = parseTerm(); // Parse the right operand
        left = std::make_unique<ComparisonExpression>(std::move(left), std::move(right), op);
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseTerm() {
    auto expr = parseFactor();
    while (currentToken.is(Token::Kind::Plus) || currentToken.is(Token::Kind::Minus)) {
        std::string op(1, currentToken.lexeme()[0]);
        consume(currentToken.kind(), "Expected '+' or '-'");
        auto right = parseFactor();
        expr = std::make_unique<TermExpression>(std::move(expr), std::move(right), op);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseFactor() {
    auto expr = parseUnary();
    while (currentToken.is(Token::Kind::Asterisk) || currentToken.is(Token::Kind::Slash)) {
        std::string op(1, currentToken.lexeme()[0]);
        consume(currentToken.kind(), "Expected '*' or '/'");
        auto right = parseUnary();
        expr = std::make_unique<FactorExpression>(std::move(expr), std::move(right), op);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseUnary() {
    if (currentToken.is(Token::Kind::Minus)) {
        consume(Token::Kind::Minus, "Expected '-'");
        auto expr = parsePrimary();
        std::string op = "-";
        return std::make_unique<UnaryExpression>(std::move(expr), op);
    }
    else if (currentToken.is(Token::Kind::Not)) {
        consume(Token::Kind::Not, "Expected 'not'");
        auto expr = parsePrimary();
        std::string op = "not";
        return std::make_unique<UnaryExpression>(std::move(expr), op);
    }
    else {
        return parsePrimary();
    }
}

std::unique_ptr<Expression> Parser::parseBinary() {
    auto left = parseUnary(); // Start with the highest precedence expressions

        // Check if a binary operation follows
    if (currentToken.is_one_of(Token::Kind::Plus, Token::Kind::Minus, Token::Kind::Asterisk, Token::Kind::Slash)) {
        std::string op = std::string(currentToken.lexeme());
        consume(currentToken.kind(), "Expected an operator");
        auto right = parseUnary(); // Assume only one binary operation is allowed
        return std::make_unique<BinaryExpression>(std::move(left), std::move(right), op);
    }

    return left; // If no binary operation, return the primary expression
}


std::unique_ptr<Expression> Parser::parsePrimary() {

    if (currentToken.is(Token::Kind::Identifier)) {
        auto expr = std::make_unique<PrimaryExpression>(std::string(currentToken.lexeme()));
        consume(Token::Kind::Identifier, "Expected identifier.");
        return expr;
    }
    else if (currentToken.is(Token::Kind::Number)) {
        auto expr = std::make_unique<PrimaryExpression>(std::string(currentToken.lexeme()));
        consume(Token::Kind::Number, "Expected number.");
        return expr;
    }

    else if (currentToken.is(Token::Kind::FloatLiteral)) {
        auto expr = std::make_unique<PrimaryExpression>(std::string(currentToken.lexeme()));
        consume(Token::Kind::FloatLiteral, "Expected string literal.");
        return expr;
    }
    else {
        throw std::runtime_error("Unexpected token in expression in parsePrimary()");
    }

    return std::make_unique<PrimaryExpression>(std::string(currentToken.lexeme()));
}

// std::unique_ptr<Statement> Parser::parseAssignmentStatement() {
    
// }

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
