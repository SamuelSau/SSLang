#include <iostream>
#include <iomanip>
#include <string>
#include <memory>

#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "ast/ASTNodes.h"

int main() {
  auto code =
  "x = 5;";

  Lexer lex(code);
  // for (auto token = lex.next(); !token.is_one_of(Token::Kind::End, Token::Kind::Unexpected); token = lex.next()) {
  //   std::cout << std::setw(12) << "Lexeme: " << token.lexeme() << "\t | Token: " << token.kind() << "\n";
  // }
  Parser parser(lex);

  //simply testing the parser with declarations
  //auto declaration = parser.parseDeclaration();
  auto expression = parser.parseExpression();
  std::cout << expression->toString() << std::endl;
  //auto statement = parser.parseStatement();
//   try {
//     auto program = parser.parseProgram();
//     program->print(std::cout);
// } catch (const std::exception& e) {
//     std::cerr << "Exception caught in parseProgram: " << e.what() << std::endl;
// } catch (...) {
//     std::cerr << "Unknown exception caught in parseProgram." << std::endl;
// }
//   std::cout << "Program parsed.\n";

  return 0;
}
