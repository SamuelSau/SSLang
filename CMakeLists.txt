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
add_executable(SSLangTests tests/test_runner.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/ast/AST.cpp)

# Custom target for running tests
add_custom_target(run_tests
    COMMAND SSLangTests
    DEPENDS SSLangTests
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)