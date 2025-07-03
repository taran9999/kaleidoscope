#pragma once

#include <string>

enum class TokenType {
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
    IDENTIFIER,
    NUMBER,
    END,
};

class Token {
public:
    Token(TokenType type, std::string data, int line, int col);
};
