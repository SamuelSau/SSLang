//#include "ast/ASTNodes.h"
//#include "parser/Parser.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include "../../include/ast/ASTNodes.h"
#include "../../include/parser/Parser.h"



std::unique_ptr<Declaration> Parser::parseIntDeclaration() {
    consume(Token::Kind::Int, "Expected 'int' for this declaration.");

    if (!currentToken.is(Token::Kind::Identifier)) {

       throw std::runtime_error("Expected variable name after 'int'.");
    }
    std::string name = std::string(currentToken.lexeme());
    consume(Token::Kind::Identifier, "Expected identifier after 'int'.");

    consume(Token::Kind::Equal, "Expected '=' after variable name.");
    
    if (!currentToken.is(Token::Kind::Number)) {
       throw std::runtime_error("Expected integer after '=' in variable declaration.");


    }
    std::string number = std::string(currentToken.lexeme());
    consume(Token::Kind::Number, "Expected integer after '='.");

    consume(Token::Kind::Semicolon, "Expected ';' after variable declaration.");
    
    return std::make_unique<IntDeclaration>(name, number);
}

std::unique_ptr<Declaration> Parser::parseFloatDeclaration() {
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


std::unique_ptr<Declaration> Parser::parseStringDeclaration(){
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
    
    return std::make_unique<StringDeclaration>(name, value);
}

std::unique_ptr<Declaration> Parser::parseBoolDeclaration(){
    consume(Token::Kind::Bool, "Expected 'bool' for this declaration.");
    if (!currentToken.is(Token::Kind::Identifier)) {
       throw std::runtime_error("Expected variable name after 'bool'.");


    }
    std::string name = std::string(currentToken.lexeme());
    consume(Token::Kind::Identifier, "Expected identifier after 'bool'.");
    consume(Token::Kind::Equal, "Expected '=' after variable name.");
    if (!currentToken.is_one_of(Token::Kind::True, Token::Kind::False)) {
       throw std::runtime_error("Expected boolean value after '=' in variable declaration.");


    }
    std::string value = std::string(currentToken.lexeme());
    consume(currentToken.kind(), "Expected boolean value after '='.");
    consume(Token::Kind::Semicolon, "Expected ';' after variable declaration.");
    return std::make_unique<BoolDeclaration>(name, value);
    
}

//Parsing expressions
std::unique_ptr<Expression> Parser::parseAssignment() {
    // Example: Parsing "x = 5;"
    if (currentToken.is(Token::Kind::Identifier)) {
        std::string variableName = std::string(currentToken.lexeme()); // Capture the variable name
        consume(Token::Kind::Identifier, "Expected an identifier"); // Move past the identifier

        if (currentToken.is(Token::Kind::Equal)) {
            consume(Token::Kind::Equal, "Expected an = after identifier"); // Consume the '=' operator
            if (currentToken.is(Token::Kind::LeftParen)) {
                consume(Token::Kind::LeftParen, "Expected '(' after parsing assignment.");
            }
            auto right = parseExpression(); // Recursively parse the right-hand side expression
            consume(Token::Kind::Semicolon, "Expected ';' after expression");

            // Create an AssignmentExpression with the variable name and the expression
            return std::make_unique<AssignmentExpression>(std::move(variableName), std::move(right));
        }
        else {
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
    if (currentToken.is_one_of(Token::Kind::Plus, Token::Kind::Minus, Token::Kind::Asterisk, Token::Kind::Slash, Token::Kind::GreaterThan, Token::Kind::LessThan, Token::Kind::GreaterThanEqual, Token::Kind::LessThanEqual, Token::Kind::NotEquals, Token::Kind::Equals, Token::Kind::And, Token::Kind::Or)) {
        std::string op = std::string(currentToken.lexeme());
        consume(currentToken.kind(), "Expected an operator");
        auto right = parseUnary(); // Assume only one binary operation is allowed
        return std::make_unique<BinaryExpression>(std::move(left), std::move(right), op);
    }

    return left; // If no binary operation, return the primary expression
}


std::unique_ptr<Expression> Parser::parsePrimary() {
    if (currentToken.is_one_of(Token::Kind::Int, Token::Kind::Float, Token::Kind::String)) {
       throw std::runtime_error("Forbidden keyword for expressions. Please use \"int\", \"flt\", or \"str\" for declarations.");
    }
    else if (currentToken.is_one_of(Token::Kind::While, Token::Kind::For)) {
       throw std::runtime_error("Forbidden keyword for expressions. Please use \"loop\" for while and for.");

    }
    if (currentToken.is(Token::Kind::Loop)){
       throw std::runtime_error("Loop keyword is not allowed in primary expressions.");

    }
    else if (currentToken.is(Token::Kind::Return)){
       throw std::runtime_error("No returning for primary expressions");


    }
    else if (currentToken.is_one_of(Token::Kind::Call, Token::Kind::Function)){
       throw std::runtime_error("No functions calls or definitions in primary expressions.");


    }
    else if (currentToken.is_one_of(Token::Kind::LeftCurly, Token::Kind::RightCurly, Token::Kind::LeftParen, Token::Kind::RightParen, Token::Kind::LeftSquare, Token::Kind::RightSquare)) {
       throw std::runtime_error("Forbidden keyword for expressions. Do not use curly braces, parentheses, or square brackets in expressions.");


    }
    else if (currentToken.is(Token::Kind::Comment)) {
        advance();
    }
    else if (currentToken.is(Token::Kind::Identifier)) {
        auto expr = std::make_unique<PrimaryExpression>(std::string(currentToken.lexeme()));
        consume(Token::Kind::Identifier, "Expected identifier.");
        return expr;
    }
    else if (currentToken.is(Token::Kind::Number)) {
        auto expr = std::make_unique<PrimaryExpression>(std::string(currentToken.lexeme()));
        consume(Token::Kind::Number, "Expected integer.");
        return expr;
    }

    else if (currentToken.is(Token::Kind::FloatLiteral)) {
        auto expr = std::make_unique<PrimaryExpression>(std::string(currentToken.lexeme()));
        consume(Token::Kind::FloatLiteral, "Expected float literal.");
        return expr;
    }

    else if (currentToken.is_one_of(Token::Kind::True, Token::Kind::False)){
        auto expr = std::make_unique<PrimaryExpression>(std::string(currentToken.lexeme()));
        consume(currentToken.kind(), "Expected boolean value.");
        return expr;
    }

    else {
        std::cout << "Token in primary expression: " << currentToken.lexeme() << "\n";
       throw std::runtime_error("Unexpected token in expression for parsing primary");
    }

    return std::make_unique<PrimaryExpression>(std::string(currentToken.lexeme()));
}

//Parsing statements
std::unique_ptr<Statement> Parser::parseLoopStatement(){

    consume(Token::Kind::Loop, "Expected 'loop'");
    if (currentToken.is(Token::Kind::Range)) {
        return parseForLoop(); 
    } 
    else if (currentToken.is(Token::Kind::LeftParen)) {
        return parseWhileLoop();
    }
    else {
       throw std::runtime_error("Expected either 'range' or '(' after 'loop'");


    }
}

std::unique_ptr<Statement> Parser::parseForLoop(){
    consume(Token::Kind::Range, "Expected 'range'");
    consume(Token::Kind::LeftParen, "Expected '(' after 'range'");
    
    auto start = parseExpression(); // Parse start of range
    consume(Token::Kind::Comma, "Expected ',' after start value");
    auto end = parseExpression(); // Parse end of range
    consume(Token::Kind::RightParen, "Expected ')' after range values");
    auto body = parseBlock(); // Parse loop body as a block of statements
    
    return std::make_unique<ForLoopStatement>(std::move(start), std::move(end), std::move(body));
}

std::unique_ptr<Statement> Parser::parseWhileLoop() {
    consume(Token::Kind::LeftParen, "Expected '(' after while 'loop'");
    auto condition = parseExpression(); // Parse loop condition
    consume(Token::Kind::RightParen, "Expected ')' after condition while loop");
    auto body = parseBlock(); // Parse loop body as a block of statements
    
    return std::make_unique<WhileLoopStatement>(std::move(condition), std::move(body));
}

//Parsing blocks
std::unique_ptr<Statement> Parser::parseBlock() {
    consume(Token::Kind::LeftCurly, "Expected '{' at start of block");

    std::vector<std::unique_ptr<Statement>> statements;

    if(currentToken.is(Token::Kind::Comment)){
        advance();
    }

    while (!currentToken.is(Token::Kind::RightCurly) && !currentToken.is(Token::Kind::End)) {
        statements.push_back(parseStatement());
    }

    consume(Token::Kind::RightCurly, "Expected '}' at end of block");

    return std::make_unique<BlockStatement>(std::move(statements));
}

std::unique_ptr<Statement> Parser::parsePrintStatement() {
    if (currentToken.is(Token::Kind::Log)) {
        consume(Token::Kind::Log, "Expected 'log' keyword.");

        consume(Token::Kind::LeftParen, "Expected '(' after 'log' keyword.");
        auto expr = parseUnary();
        
        if (currentToken.is(Token::Kind::Comma) || currentToken.is_one_of(Token::Kind::Plus, Token::Kind::Minus, Token::Kind::Asterisk, Token::Kind::Slash)) {
           throw std::runtime_error("Only expected 1 expression to log.");


        }

        consume(Token::Kind::RightParen, "Expected ')' after logging expression.");

        consume(Token::Kind::Semicolon, "Expected ';' after logging expression.");
        return std::make_unique<PrintStatement>(std::move(expr));
    }
    else {
       throw std::runtime_error("Unexpected token: Expected 'log' keyword.");


    }
}

std::unique_ptr<Statement> Parser::parseReturnStatement() {
    consume(Token::Kind::Return, "Expected 'ret' keyword.");
    consume(Token::Kind::LeftParen, "Expected '(' after 'ret' keyword.");
    auto expr = parseUnary();

    if (currentToken.is_one_of(Token::Kind::Plus, Token::Kind::Minus, Token::Kind::Asterisk, Token::Kind::Slash) || currentToken.is(Token::Kind::Comma)){
       throw std::runtime_error("Expected 1 expression after 'ret' keyword.");


    }
    consume(Token::Kind::RightParen, "Expected ')' after returning expression.");
    consume(Token::Kind::Semicolon, "Expected ';' after returning expression.");
    return std::make_unique<ReturnStatement>(std::move(expr));
}

std::unique_ptr<Statement> Parser::parseIfStatement() {
    if (!currentToken.is(Token::Kind::If)){
       throw std::runtime_error("Expected 'if' keyword.");
    }
    consume(Token::Kind::If, "Expected 'if' keyword.");
    if (!currentToken.is(Token::Kind::LeftParen)){
       throw std::runtime_error("Expected '(' after 'if' keyword.");
    }
    consume(Token::Kind::LeftParen, "Expected '(' after 'if' keyword.");
    auto condition = parseExpression();

    consume(Token::Kind::RightParen, "Expected ')' after if condition.");
    auto thenBody = parseBlock();

    std::unique_ptr<Statement> elseBody = nullptr;

    if (currentToken.is(Token::Kind::Else)) {
        consume(Token::Kind::Else, "Expected 'else' keyword.");
        elseBody = parseBlock(); 
    }

    return std::make_unique<IfStatement>(std::move(condition), std::move(thenBody), std::move(elseBody));
}


//Parsing functions
std::unique_ptr<Function> Parser::parseFunctionDefinition() {
    consume(Token::Kind::Function, "Expected 'function' keyword.");
    std::string name = std::string(currentToken.lexeme());
    consume(Token::Kind::Identifier, "Expected function name to be identifer");
    
    // Parameter parsing
    std::vector<ParamInfo> parameters;
    consume(Token::Kind::LeftParen, "Expected '(' after function name.");
    while (!currentToken.is(Token::Kind::RightParen)) {
        
        std::string paramType = std::string(currentToken.lexeme());
        if (currentToken.is_one_of(Token::Kind::Int, Token::Kind::Float, Token::Kind::String)) {
            consume(currentToken.kind(), "Expected parameter type of which can be int, flt, or str.");
        }
        consume(Token::Kind::Colon, "Expected colon after parameter type.");

        std::string paramName = std::string(currentToken.lexeme());
        consume(Token::Kind::Identifier, "Expected parameter name to be identifier");

        parameters.emplace_back(paramName, paramType);

        if (currentToken.is(Token::Kind::Comma)) {
            consume(Token::Kind::Comma, "Expected ',' between parameters.");
        } 
        
        else if (!currentToken.is(Token::Kind::RightParen)) {
           throw std::runtime_error("Expected ',' or ')' after parameter.");


        }
    }
    consume(Token::Kind::RightParen, "Expected ')' after parameters.");

    consume(Token::Kind::Arrow, "Expected '->' after parameters.");
    std::string returnType = std::string(currentToken.lexeme());
    if (currentToken.is_one_of(Token::Kind::Int, Token::Kind::Float, Token::Kind::String)) {
        consume(currentToken.kind(), "Expected return type after '->' of which can be int, flt, or str.");
    } else {
       throw std::runtime_error("Expected return type of either int, flt, or str");


    }
    // Function body parsing
    std::vector<std::unique_ptr<Statement>> body;
    consume(Token::Kind::LeftCurly, "Expected '{' before function body.");
    
    while (!currentToken.is(Token::Kind::RightCurly) && !currentToken.is(Token::Kind::End)) {
        body.push_back(parseStatement()); // Parse each statement in the body
    }
    consume(Token::Kind::RightCurly, "Expected '}' after function body.");

    return std::make_unique<FunctionDefinition>(name, parameters, returnType, std::move(body));
}

std::unique_ptr<Function> Parser::parseFunctionCall() {
    consume(Token::Kind::Call, "Expected 'call' keyword.");
    std::vector<std::unique_ptr<Expression>> arguments;
     std::string name = std::string(currentToken.lexeme());
    consume(Token::Kind::Identifier, "Expected function name to be identifier");
    consume(Token::Kind::LeftParen, "Expected '(' after function name.");
   
    while (!currentToken.is(Token::Kind::RightParen)) {
        auto arg = parseExpression();
        arguments.push_back(std::move(arg));
        
        if (currentToken.is(Token::Kind::Comma)) {
            consume(Token::Kind::Comma, "Expected ',' between arguments.");
        } else if (!currentToken.is(Token::Kind::RightParen)) {
           throw std::runtime_error("Expected ',' or ')' after argument.");


        }
    }
    consume(Token::Kind::RightParen, "Expected ')' after arguments.");
    consume(Token::Kind::Semicolon, "Expected ';' after function call.");
    return std::make_unique<FunctionCall>(name, std::move(arguments));
}

std::unique_ptr<Program> Parser::parseProgram() {

    auto program = std::make_unique<Program>();
    std::vector<std::unique_ptr<Function>> functions;
    std::vector<std::unique_ptr<Declaration>> declarations;
    std::vector<std::unique_ptr<Statement>> statements;
    std::vector<std::unique_ptr<Expression>> expressions;

    while (!currentToken.is(Token::Kind::End)) {
        if (currentToken.is_one_of(Token::Kind::Function, Token::Kind::Call)) {
            program->functions.push_back(parseFunction());
        }
        else if (currentToken.is_one_of(Token::Kind::Int, Token::Kind::Float, Token::Kind::String, Token::Kind::Bool)) {
            program->declarations.push_back(parseDeclaration());
        } 
        else if (currentToken.is_one_of(Token::Kind::Log, Token::Kind::Return, Token::Kind::If, Token::Kind::Loop, Token::Kind::Else)) {
            program->statements.push_back(parseStatement());
        }
        else {
            program->expressions.push_back(parseExpression());
        }
    }

    return program;
}
