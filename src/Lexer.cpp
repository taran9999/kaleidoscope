#include "Lexer.hpp"
#include "Token.hpp"
#include <cctype>

Lexer::Lexer(std::ifstream& f) : f(f) {}

Token Lexer::NextToken() {
    if(c == EOF) return Token(TokenType::END, "", line, col);

    c = f.get();
    col++;

    if(std::isalpha(c)) return TokenizeIdentifier();
    else if(std::isdigit(c)) return TokenizeNumber();

    else if(c == '+') return Token(TokenType::PLUS, "", line, col);
    else if(c == '-') return Token(TokenType::MINUS, "", line, col);

    else if(c == '(') return Token(TokenType::LPAR, "", line, col);
    else if(c == ')') return Token(TokenType::RPAR, "", line, col);
    else if(c == ';') return Token(TokenType::SEMICOLON, "", line, col);

    else if(c == '\n') {
        line++;
        col = 0;
        return NextToken();
    }

    else return NextToken();
}
