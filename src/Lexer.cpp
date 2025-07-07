#include "Lexer.hpp"
#include "Token.hpp"
#include <cctype>
#include <string>

Lexer::Lexer(std::ifstream& f) : f(f), line(1), col(0), first(false) {}

void Lexer::NextChar() {
    if(c == EOF) return;

    c = f.get();
    col++;

    if(c == '\n') {
        line++;
        col = 0;
    }
}

Token Lexer::TokenizeIdentifier() {
    std::string data;
    unsigned int curr_line = line;
    unsigned int curr_col = col;

    while(std::isalnum(c)) {
        data += c;
        NextChar();
    }

    first = true;

    if(data == "def") return Token(TokenType::DEF, "", curr_line, curr_col);
    else if(data == "if") return Token(TokenType::IF, "", curr_line, curr_col);
    else if(data == "then") return Token(TokenType::THEN, "", curr_line, curr_col);
    else if(data == "else") return Token(TokenType::ELSE, "", curr_line, curr_col);
    else if(data == "for") return Token(TokenType::FOR, "", curr_line, curr_col);
    else if(data == "extern") return Token(TokenType::EXTERN, "", curr_line, curr_col);
    else return Token(TokenType::IDENTIFIER, data, curr_line, curr_col);
}

Token Lexer::TokenizeNumber() {
    std::string data;
    unsigned int curr_line = line;
    unsigned int curr_col = col;

    while(std::isdigit(c) || c == '.') {
        data += c;
        NextChar();
    }

    first = true;

    return Token(TokenType::NUMBER, data, curr_line, curr_col);
}

Token Lexer::NextToken() {
    if(c == EOF) return Token(TokenType::END, "", line, col);

    if(!first) NextChar();
    else first = false;

    if(std::isalpha(c)) return TokenizeIdentifier();
    else if(std::isdigit(c)) return TokenizeNumber();

    else if(c == '+') return Token(TokenType::PLUS, "", line, col);
    else if(c == '-') return Token(TokenType::MINUS, "", line, col);

    else if(c == '(') return Token(TokenType::LPAR, "", line, col);
    else if(c == ')') return Token(TokenType::RPAR, "", line, col);
    else if(c == ';') return Token(TokenType::SEMICOLON, "", line, col);

    else if(c == '<') {
        if(f.peek() == '=') {
            NextChar();
            return Token(TokenType::LE, "", line, col);
        }
        else return Token(TokenType::LT, "", line, col);
    }

    else if(c == '>') {
        if(f.peek() == '=') {
            NextChar();
            return Token(TokenType::GE, "", line, col);
        }
        else return Token(TokenType::GT, "", line, col);
    }

    else return NextToken();
}
