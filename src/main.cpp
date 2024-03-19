#include "lexer/Lexer.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <cstddef>
#include <iterator>

int main() {
  auto code =
      "x = 2\n"
      "// This is a comment.\n"
      "// This is another comment.\n"
      "var x\n"
      "var y\n"
      "var f = function(x, y) { sin(x) * sin(y) + x * y; }\n"
      "der(f, x)\n"
      "var g = function(x, y) { 2 * (x + der(f, y)); } // der(f, y) is a "
      "matrix\n"
      "var r{3}; // Vector of three elements\n"
      "var J{12, 12}; // Matrix of 12x12 elements\n"
      "var dot = function(u{:}, v{:}) -> scalar {\n"
      "          return u[i] * v[i]; // Einstein notation\n"
      "}\n"
      "var norm = function(u{:}) -> scalar { return sqrt(dot(u, u)); }\n"
      "<end>";

  Lexer lex(code);
  for (auto token = lex.next(); !token.is_one_of(Token::Kind::End, Token::Kind::Unexpected);
       token = lex.next()) {
    std::cout << std::setw(12) << token.kind() << " |" << token.lexeme()
              << "|\n";
  }
}