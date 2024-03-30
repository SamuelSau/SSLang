cmake_minimum_required(VERSION 3.23)

# Project Name
project(SSLang)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(debugEnable ON)
else()
  set(debugEnable OFF)
endif()

option(BUILD_UTILS "Build utilities" ${debugEnable})
option(ENABLE_TESTS "Enable tests" ${debugEnable})

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Find LLVM
set(LLVM_DIR "C:/Users/Sammy/llvm-project/llvm/build/bootstrap/build/lib/cmake/llvm")
find_package(LLVM REQUIRED CONFIG)
message(STATUS "FOUND LLVM ${LLVM_PACKAGE_VERSION}: ${LLVM_INSTALL_PREFIX}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

# Plugins have to be explicitly enabled for Windows.
set(LLVM_ENABLE_PLUGINS ON)

# Disable RTTI if LLVM was build without it.
if (NOT LLVM_ENABLE_RTTI)
  if (MSVC)
    string(APPEND CMAKE_CXX_FLAGS " /GR-")
  else()
    string(APPEND CMAKE_CXX_FLAGS " -fno-rtti")
  endif()
endif()

# Include Directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    #including llvm
    ${LLVM_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include
    )

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

if (APPLE)
  link_libraries("$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")
endif()

llvm_map_components_to_libnames(llvmLibs Core Passes Support)

# Main Executable
add_executable(SSLang src/main.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/ast/AST.cpp src/symbolTable/SymbolTable.cpp src/semanticAnalyzer/SemanticAnalyzer.cpp src/llvmGen/LLVMCodeGen.cpp src/llvmGen/LLVMUtility.cpp)

# Test Executables
add_executable(SSLangDeclareTests tests/declaration_testing/declaration_test_runner.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/ast/AST.cpp src/symbolTable/SymbolTable.cpp src/semanticAnalyzer/SemanticAnalyzer.cpp)
add_executable(SSLangExpressionTests tests/expression_testing/expression_test_runner.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/ast/AST.cpp src/symbolTable/SymbolTable.cpp src/semanticAnalyzer/SemanticAnalyzer.cpp)
add_executable(SSLangStatementTests tests/statement_testing/statement_test_runner.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/ast/AST.cpp src/symbolTable/SymbolTable.cpp src/semanticAnalyzer/SemanticAnalyzer.cpp)
add_executable(SSLangFunctionTests tests/function_testing/function_test_runner.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/ast/AST.cpp src/symbolTable/SymbolTable.cpp src/semanticAnalyzer/SemanticAnalyzer.cpp)
add_executable(SSLangProgramTests tests/program_testing/program_test_runner.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/ast/AST.cpp src/symbolTable/SymbolTable.cpp src/semanticAnalyzer/SemanticAnalyzer.cpp)

if(MSVC)
    target_compile_options(SSLang PRIVATE /EHsc)
    target_compile_options(SSLangDeclareTests PRIVATE /EHsc)
    target_compile_options(SSLangExpressionTests PRIVATE /EHsc)
    target_compile_options(SSLangStatementTests PRIVATE /EHsc)
    target_compile_options(SSLangFunctionTests PRIVATE /EHsc)
    target_compile_options(SSLangProgramTests PRIVATE /EHsc)
else()
    target_compile_options(SSLang PRIVATE -fexceptions)
    target_compile_options(SSLangDeclareTests PRIVATE -fexceptions)
    target_compile_options(SSLangExpressionTests PRIVATE -fexceptions)
    target_compile_options(SSLangStatementTests PRIVATE -fexceptions)
    target_compile_options(SSLangFunctionTests PRIVATE -fexceptions)
    target_compile_options(SSLangProgramTests PRIVATE -fexceptions)
endif()

target_link_libraries(SSLang PRIVATE ${llvmLibs})

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

add_custom_target(run_program_tests
    COMMAND SSLangProgramTests
    DEPENDS SSLangProgramTests
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)