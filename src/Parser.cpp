#include <iostream>

#include "Parser.hpp"

Token Parser::current() {
    return at_end() ? end_token : tokens[pos];
}

Token Parser::lookahead(std::size_t ofs) {
    size_t new_pos = pos + ofs;
    return new_pos < tokens.size() ? tokens[new_pos] : end_token;
}

Token Parser::accept(TokenType expected) {
    if(!check(expected)) error(expected);

    Token curr = current();
    advance();
    return curr;
}

bool Parser::check(TokenType expected) {
    Token curr = current();
    return curr.type == expected;
}

bool Parser::at_end() {
    return pos < tokens.size();
}

void Parser::advance() {
    if(!at_end()) pos++;
}

void Parser::error(TokenType expected) {
    Token curr = current();
    std::cout << "Got " << string_of_token_type(curr.type) << " at " << curr.line << ":" << curr.col << " (expected " << string_of_token_type(expected) << ")" << std::endl;
    num_errors++;
}
