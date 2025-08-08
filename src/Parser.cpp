#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "Parser.hpp"
#include "Token.hpp"

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

std::unique_ptr<Program> Parser::parseProgram() {
    std::vector<std::unique_ptr<FuncDef>> functions;

    while(!at_end()) {
        auto f = parseFuncDef();
        functions.push_back(std::move(f));
    }

    return std::make_unique<Program>(std::move(functions));
}

std::unique_ptr<FuncDef> Parser::parseFuncDef() {
    accept(TokenType::DEF);
    Token nameToken = accept(TokenType::IDENTIFIER);
    auto name = std::move(nameToken.data);

    std::vector<std::string> params;
    while(check(TokenType::IDENTIFIER)) {
        Token curr = current();
        params.push_back(std::move(curr.data));
        advance();
    }

    accept(TokenType::ARROW);
    auto block = parseBlock();
    accept(TokenType::END);

    return std::make_unique<FuncDef>(std::move(name), std::move(params), std::move(block));
}
