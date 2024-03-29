#include <string>
#include <cstddef>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <string_view>
#include <unordered_map>

//#include "lexer/Lexer.h"
#include "../../include/lexer/Lexer.h"

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
    if (std::isdigit(peek())) {
      return number();
    }
    if (std::isalpha(peek())) {
      return identifier();
    }
    case '\0':
      return Token(Token::Kind::End, m_beg, 1);
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
       if (std::next(m_beg) != '\0' && *std::next(m_beg) == '>') {
                m_beg += 2; // 
                return Token(Token::Kind::Arrow, token_start, m_beg);
          }
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
      return string_literal();
    case '|':
      return atom(Token::Kind::Pipe);
    default:
      if (is_identifier_char(peek())) {
            return identifier();
            } 
      else if (is_digit(peek())) {
            return number();
            }       
      else if (!isprint(peek())) { // Check for non-printable characters
            std::cerr << "Error: Unrecognized non-printable character encountered at position " << std::distance(m_original_beg, token_start) << ".\n";
            return Token(Token::Kind::Unexpected, token_start, 1); // Use the original character position for error message
            } 
     else {
            std::cerr << "Error: Unrecognized character '" << peek() << "' encountered at position " << std::distance(m_original_beg, token_start) << ".\n";
            return Token(Token::Kind::Unexpected, token_start, 1); // Use the original character position for error message
          }
  }
}

Token Lexer::identifier() noexcept {
   const char* start = m_beg;
    while (is_identifier_char(peek())) get();
    
    // Example keyword checking, needs to be expanded based on actual language keywords
    std::string_view text(start, std::distance(start, m_beg));
    static const std::unordered_map<std::string_view, Token::Kind> keywords = {
        {"function", Token::Kind::Function},
        {"if", Token::Kind::If},
        {"else", Token::Kind::Else},
        {"ret", Token::Kind::Return},
        {"loop", Token::Kind::Loop},
        {"int", Token::Kind::Int},
        {"flt", Token::Kind::Float},
        {"str", Token::Kind::String},
        {"bool", Token::Kind::Bool},
        {"log", Token::Kind::Log},
        {"not", Token::Kind::Not},
        {"equals", Token::Kind::Equals},
        {"notEquals", Token::Kind::NotEquals},
        {"or", Token::Kind::Or},
        {"and", Token::Kind::And},
        {"range", Token::Kind::Range},
        {"for", Token::Kind::For},
        {"while", Token::Kind::While},
        {"print", Token::Kind::Print},
        {"call", Token::Kind::Call},
        {"true", Token::Kind::True},
        {"false", Token::Kind::False}

    };

    auto keyword = keywords.find(text);
    if (keyword != keywords.end()) {
        return Token(keyword->second, start, std::distance(start, m_beg));
    }
    return Token(Token::Kind::Identifier, start, std::distance(start, m_beg));
 }


Token Lexer::number() noexcept {
  const char* start = m_beg;
  get();
  while (is_digit(peek())) get();
  bool isFloat = false;
    if (peek() == '.') {
        isFloat = true;
        get(); // Consume the '.'
        while (std::isdigit(peek())) get(); // Consume the fractional part
    }

    if (isFloat) {
        // We have a floating-point number
        return Token(Token::Kind::FloatLiteral, start, std::distance(start, m_beg));
    } else {
        // It's an integer
        return Token(Token::Kind::Number, start, std::distance(start, m_beg));
    }
}

Token Lexer::slash_or_comment() noexcept {
    get(); // Consume the first slash
    if (peek() == '/') { // Confirm it's a line comment
        while (peek() != '\0' && peek() != '\n') {
          get(); // Consume all characters until the end of line or end of file
        }
        return next(); // Recursively call next() to get the next valid token after the comment
    } 
    
    else {
        return Token(Token::Kind::Slash, m_beg - 1, 1); // If it's not a comment, return the slash token
    }
}


Token Lexer::string_literal() noexcept {
    const char* start = ++m_beg; // Skip the initial double-quote

    while (*m_beg != '"' && *m_beg != '\0') {
        // Handle escaped characters if your language syntax supports them
        if (*m_beg == '\\' && *(m_beg + 1) == '"') {
            m_beg++; // Skip the escape character
        }
        m_beg++;
    }

    if (*m_beg == '"') {
        
        Token token(Token::Kind::StringLiteral, start, m_beg);
        m_beg++; // Skip the closing double-quote
        return token;
    } else {
        return Token(Token::Kind::Unexpected, start, m_beg);
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
  bool result = false;

  if (c >= '0' && c <= '9') {
    result = true;
  }
  return result;
}

bool is_identifier_char(char c) noexcept {
  bool result = false;
  if (c >= 'a' && c <= 'z') {
    result = true;
  } else if (c >= 'A' && c <= 'Z') {
    result = true;
  } else if (c == '_') {
    result = true;
  }
  return result;
}
