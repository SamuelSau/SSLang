#include <iostream>
#include <iomanip>
#include <string>
#include <memory>

#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "ast/ASTNodes.h"
#include "symbolTable/SymbolTable.h"
#include "semanticAnalyzer/SemanticAnalyzer.h"

int main() {
  auto code =
  "int x = 10;";

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

  std::cout <<  "Program parsed successfully" << std::endl;

  SymbolTable symbolTable;
  
  symbolTable.enterScope();

  //symbolTable.addVariable("x", "int");
  symbolTable.addVariable("y", "int");

  if (!symbolTable.addVariable("x", "int")) {
    std::cout << "Variable x already declared" << std::endl;
    std::cout << "Variable x is declared in scope: " << symbolTable.getSymbolInfo("x")->scopeId << std::endl;
  }
  else {
    std::cout << "Variable is not declared" << std::endl;
  }

  symbolTable.leaveScope();

  return 0;
}
