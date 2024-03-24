#include <iostream>
#include <iomanip>
#include <string>
#include <memory>

#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "ast/ASTNodes.h"
#include "symbolTable/SymbolTable.h"
#include "semanticAnalyzer/SemanticAnalyzer.h"
#include "visitor/Visitor.h"

int main() {
  auto code =
  "int y = 20;\n"
  "y = y + 10;\n"
  ;

  Lexer lexer(code);
  Parser parser(lexer);

  auto program = parser.parseProgram();

  if (program) {
        std::cout << program->toString() << std::endl;
  } 
  else {
        std::cout << "Parsing failed." << std::endl;
  }

  parser.parseProgram();


  SymbolTable symbolTable;
  
  symbolTable.enterScope();

  //symbolTable.addVariable("x", "int");
  // symbolTable.addVariable("y", "int");

  // if (!symbolTable.addVariable("x", "int")) {
  //   std::cout << "Variable x already declared" << std::endl;
  //   std::cout << "Variable x is declared in scope: " << symbolTable.getSymbolInfo("x")->scopeId << std::endl;
  // }
  // else {
  //   std::cout << "Variable is not declared" << std::endl;
  // }

  SemanticAnalyzer semanticAnalyzer(symbolTable);
  semanticAnalyzer.visit(program.get());

  std::cout << "Program is semantically correct." << std::endl;

  symbolTable.leaveScope();

  return 0;
}
