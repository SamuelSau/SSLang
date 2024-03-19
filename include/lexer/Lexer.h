// Lexer.h
#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <string_view>

class Token {
public:
    enum class Kind {
        Number, Identifier, LeftParen, RightParen, LeftSquare, RightSquare,
        LeftCurly, RightCurly, LessThan, GreaterThan, Equal, Plus, Minus,
        Asterisk, Slash, Hash, Dot, Comma, Colon, Semicolon, SingleQuote,
        DoubleQuote, Comment, Pipe, End, Unexpected,
    };

    Token(Kind kind) noexcept;
    Token(Kind kind, const char* beg, std::size_t len) noexcept;
    Token(Kind kind, const char* beg, const char* end) noexcept;

    Kind kind() const noexcept;
    void kind(Kind kind) noexcept;
    bool is(Kind kind) const noexcept;
    bool is_not(Kind kind) const noexcept;
    bool is_one_of(Kind k1, Kind k2) const noexcept;

    template <typename... Ts>
    bool is_one_of(Kind k1, Kind k2, Ts... ks) const noexcept;

    std::string_view lexeme() const noexcept;
    void lexeme(std::string_view lexeme) noexcept;

private:
    Kind m_kind{};
    std::string_view m_lexeme{};
};

class Lexer {
public:
    Lexer(const char* beg) noexcept : m_beg{beg}, m_original_beg{beg} {} // Updated constructor

    Token next() noexcept;

private:
    Token identifier() noexcept;
    Token number() noexcept;
    Token slash_or_comment() noexcept;
    Token atom(Token::Kind) noexcept;

    char peek() const noexcept { return *m_beg; }
    char get() noexcept { return *m_beg++; }
    const char* m_beg = nullptr;
    const char* m_original_beg = nullptr; // Added this line

};

bool is_space(char c) noexcept;
bool is_digit(char c) noexcept;
bool is_identifier_char(char c) noexcept;

std::ostream& operator<<(std::ostream& os, const Token::Kind& kind);

#endif // LEXER_H
