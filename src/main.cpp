#include <iostream>
#include <iomanip>
#include <string>
#include <memory>

#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "ast/ASTNodes.h"

int main() {
  auto code =
  "str x = \"Hello World\";";

  Lexer lex(code);
  // for (auto token = lex.next(); !token.is_one_of(Token::Kind::End, Token::Kind::Unexpected); token = lex.next()) {
  //   std::cout << std::setw(12) << "Lexeme: " << token.lexeme() << "\t | Token: " << token.kind() << "\n";
  // }
  Parser parser(lex);

  //simply testing the parser with declarations
  auto declaration = parser.parseDeclaration();

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
