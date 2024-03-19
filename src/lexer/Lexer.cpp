#include <string>
#include "lexer/Lexer.h"
#include <cstddef>
#include <iterator>
#include <iostream>
#include <iomanip>

Token::Token(Kind kind) noexcept : m_kind{kind} {}

Token::Token(Kind kind, const char* beg, std::size_t len) noexcept
    : m_kind{kind}, m_lexeme(beg, len) {}

Token::Token(Kind kind, const char* beg, const char* end) noexcept
    : m_kind{kind}, m_lexeme(beg, std::distance(beg, end)) {}

Token::Kind Token::kind() const noexcept { return m_kind; }

void Token::kind(Kind kind) noexcept { m_kind = kind; }

bool Token::is(Kind kind) const noexcept { return m_kind == kind; }

bool Token::is_not(Kind kind) const noexcept { return m_kind != kind; }

bool Token::is_one_of(Kind k1, Kind k2) const noexcept { return is(k1) || is(k2); }

template <typename... Ts>
bool Token::is_one_of(Kind k1, Kind k2, Ts... ks) const noexcept {
    return is(k1) || is_one_of(k2, ks...);
}

std::string_view Token::lexeme() const noexcept {
    return m_lexeme;
}

void Token::lexeme(std::string_view lexeme) noexcept {
    m_lexeme = std::move(lexeme);
}

Token Lexer::atom(Token::Kind kind) noexcept { return Token(kind, m_beg++, 1); }

Token Lexer::next() noexcept {
  while (is_space(peek())) get();

  const char* token_start = m_beg;

  switch (peek()) {
    case '\0':
      return Token(Token::Kind::End, m_beg, 1);
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
      return identifier();
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return number();
    case '(':
      return atom(Token::Kind::LeftParen);
    case ')':
      return atom(Token::Kind::RightParen);
    case '[':
      return atom(Token::Kind::LeftSquare);
    case ']':
      return atom(Token::Kind::RightSquare);
    case '{':
      return atom(Token::Kind::LeftCurly);
    case '}':
      return atom(Token::Kind::RightCurly);
    case '<':
      return atom(Token::Kind::LessThan);
    case '>':
      return atom(Token::Kind::GreaterThan);
    case '=':
      return atom(Token::Kind::Equal);
    case '+':
      return atom(Token::Kind::Plus);
    case '-':
      return atom(Token::Kind::Minus);
    case '*':
      return atom(Token::Kind::Asterisk);
    case '/':
      return slash_or_comment();
    case '#':
      return atom(Token::Kind::Hash);
    case '.':
      return atom(Token::Kind::Dot);
    case ',':
      return atom(Token::Kind::Comma);
    case ':':
      return atom(Token::Kind::Colon);
    case ';':
      return atom(Token::Kind::Semicolon);
    case '\'':
      return atom(Token::Kind::SingleQuote);
    case '"':
      return atom(Token::Kind::DoubleQuote);
    case '|':
      return atom(Token::Kind::Pipe);
    default:
      if (is_identifier_char(peek())) {
                return identifier();
            } else if (is_digit(peek())) {
                return number();
            } else if (!isprint(peek())) { // Check for non-printable characters
                std::cerr << "Error: Unrecognized non-printable character encountered at position "
                          << std::distance(m_original_beg, token_start) << ".\n";
                return Token(Token::Kind::Unexpected, token_start, 1); // Use the original character position for error message
            } else {
                std::cerr << "Error: Unrecognized character '" << peek() << "' encountered at position "
                          << std::distance(m_original_beg, token_start) << ".\n";
                return Token(Token::Kind::Unexpected, token_start, 1); // Use the original character position for error message
            }
  }
}

Token Lexer::identifier() noexcept {
  const char* start = m_beg;
  get();
  while (is_identifier_char(peek())) get();
  return Token(Token::Kind::Identifier, start, m_beg);
}

Token Lexer::number() noexcept {
  const char* start = m_beg;
  get();
  while (is_digit(peek())) get();
  return Token(Token::Kind::Number, start, m_beg);
}

Token Lexer::slash_or_comment() noexcept {
  const char* start = m_beg;
  get();
  if (peek() == '/') {
    get();
    start = m_beg;
    while (peek() != '\0') {
      if (get() == '\n') {
        return Token(Token::Kind::Comment, start,
                     std::distance(start, m_beg) - 1);
      }
    }
    return Token(Token::Kind::Unexpected, m_beg, 1);
  } else {
    return Token(Token::Kind::Slash, start, 1);
  }
}

bool is_space(char c) noexcept {
  switch (c) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
      return true;
    default:
      return false;
  }
}

bool is_digit(char c) noexcept {
  switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return true;
    default:
      return false;
  }
}

bool is_identifier_char(char c) noexcept {
  switch (c) {
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '_':
      return true;
    default:
      return false;
  }
}

std::ostream& operator<<(std::ostream& os, const Token::Kind& kind) {
  static const char* const names[]{
      "Number",      "Identifier",  "LeftParen",  "RightParen", "LeftSquare",
      "RightSquare", "LeftCurly",   "RightCurly", "LessThan",   "GreaterThan",
      "Equal",       "Plus",        "Minus",      "Asterisk",   "Slash",
      "Hash",        "Dot",         "Comma",      "Colon",      "Semicolon",
      "SingleQuote", "DoubleQuote", "Comment",    "Pipe",       "End",
      "Unexpected",
  };
  return os << names[static_cast<int>(kind)];
}
