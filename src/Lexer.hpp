#pragma once

#include "Token.hpp"
#include <fstream>

class Lexer {
private:
    char c;
    std::ifstream& f;
    unsigned int line;
    unsigned int col;
    bool first;

    void NextChar();
    Token TokenizeIdentifier();
    Token TokenizeNumber();

public:
    Lexer(std::ifstream& f);
    Token NextToken();
};
