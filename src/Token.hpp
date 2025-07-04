#pragma once

#include <string>

enum class TokenType {
    IDENTIFIER,
    NUMBER,

    // keywords
    DEF,
    IF,
    ELSE,
    FOR,
    EXTERN,

    // binary operators
    PLUS,
    MINUS,
    LT,
    LE,
    GT,
    GE,

    // misc
    LPAR,
    RPAR,
    SEMICOLON,
    END,
};

class Token {
public:
    Token(TokenType type, std::string data, int line, int col);
};
