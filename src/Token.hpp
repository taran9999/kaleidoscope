#pragma once

#include <string>

#define TOKEN_TYPES \
    X(IDENTIFIER) \
    X(NUMBER) \
    X(DEF) \
    X(IF) \
    X(THEN) \
    X(ELSE) \
    X(LOOP) \
    X(RANGE) \
    X(COMMA) \
    X(EXTERN) \
    X(PLUS) \
    X(MINUS) \
    X(LT) \
    X(LE) \
    X(GT) \
    X(GE) \
    X(LPAR) \
    X(RPAR) \
    X(SEMICOLON) \
    X(END) \
    X(ARROW) \
    X(VAR) \
    X(ASSIGN) \
    X(END_PROG)

#define X(name) name,
enum class TokenType {
    TOKEN_TYPES
};
#undef X

const char* string_of_token_type(TokenType type);

class Token {
public:
    TokenType type;
    std::string data;
    unsigned int line;
    unsigned int col;

    Token(TokenType type, std::string data, unsigned int line, unsigned int col);
    std::string to_string();
};
