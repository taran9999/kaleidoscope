#include "Token.hpp"

#include <string>

Token::Token(TokenType type, std::string data, unsigned int line, unsigned int col) 
    : type(type), data(data), line(line), col(col) {}

std::string Token::to_string() {
    std::string s;

    s += std::to_string(line);
    s += ":";
    s += std::to_string(col);
    s += '\t';

    s += string_of_token_type(type);

    if(!data.empty()) {
        s += '(';
        s += data;
        s += ')';
    }

    return s;
}

#define X(name) case TokenType::name: return #name;
const char* string_of_token_type(TokenType type) {
    switch(type) { TOKEN_TYPES }
    return "unknown";
}
#undef X
