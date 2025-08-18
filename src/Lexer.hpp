#pragma once

#include "Token.hpp"
#include <istream>

class Lexer {
private:
    char c;
    std::istream& f;
    unsigned int line;
    unsigned int col;
    bool first;

    void NextChar();
    Token TokenizeIdentifier();
    Token TokenizeNumber();

public:
    explicit Lexer(std::istream& f);
    Token NextToken();
};
