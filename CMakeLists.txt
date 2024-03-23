cmake_minimum_required(VERSION 3.10)

# Project Name
project(SSLang)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include Directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    )

# Main Executable
add_executable(SSLang src/main.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/ast/AST.cpp)

# Test Executable
add_executable(SSLangDeclareTests tests/declaration_testing/declaration_test_runner.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/ast/AST.cpp)
add_executable(SSLangExpressionTests tests/expression_testing/expression_test_runner.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/ast/AST.cpp)
add_executable(SSLangStatementTests tests/statement_testing/statement_test_runner.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/ast/AST.cpp)
add_executable(SSLangFunctionTests tests/function_testing/function_test_runner.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/ast/AST.cpp)

# Custom target for running tests
add_custom_target(run_declare_tests
    COMMAND SSLangDeclareTests
    DEPENDS SSLangDeclareTests
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
add_custom_target(run_expression_tests
    COMMAND SSLangExpressionTests
    DEPENDS SSLangExpressionTests
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(run_statement_tests
    COMMAND SSLangStatementTests
    DEPENDS SSLangStatementTests
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(run_function_tests
    COMMAND SSLangFunctionTests
    DEPENDS SSLangFunctionTests
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
