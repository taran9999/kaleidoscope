#pragma once
#include <algorithm>
#include <cstdlib>
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

bool Parser::checkExpr() {
    return
        check(TokenType::IF) ||
        check(TokenType::NUMBER) ||
        check(TokenType::IDENTIFIER);
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

std::unique_ptr<Block> Parser::parseBlock() {
    std::vector<std::unique_ptr<Expr>> exprs;

    while(checkExpr()) {
        auto e = parseExpr();
        exprs.push_back(std::move(e));
    }

    return std::make_unique<Block>(std::move(exprs));
}

std::unique_ptr<Expr> Parser::parseExpr() {
    if(check(TokenType::IF)) return parseIfExpr();
    else return parseExpr2();
}

std::unique_ptr<IfExpr> Parser::parseIfExpr() {
    accept(TokenType::IF);
    auto cond = parseExpr();
    accept(TokenType::THEN);
    auto block1 = parseBlock();
    accept(TokenType::ELSE);
    auto block2 = parseBlock();
    accept(TokenType::END);

    return std::make_unique<IfExpr>(std::move(cond), std::move(block1), std::move(block2));
}

std::unique_ptr<VarExpr> Parser::parseVarExpr() {
    Token curr = current();
    auto name = std::move(curr.data);
    return std::make_unique<VarExpr>(std::move(name));
}

std::unique_ptr<NumLiteral> Parser::parseNumLiteral() {
    Token curr = current();
    std::string data = std::move(curr.data);
    auto val = std::stoi(data);
    return std::make_unique<NumLiteral>(val);
}

std::unique_ptr<Expr> Parser::parseExpr2() {
    auto lhs = parseExpr1();

    while(check(TokenType::LT)) {
        accept(TokenType::LT);
        auto rhs = parseExpr1();
        lhs = std::make_unique<BinOp>(std::move(lhs), '<', std::move(rhs));
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::parseExpr1() {
    auto lhs = parseExpr0();

    while(check(TokenType::MINUS)) {
        accept(TokenType::MINUS);
        auto rhs = parseExpr0();
        lhs = std::make_unique<BinOp>(std::move(lhs), '-', std::move(rhs));
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::parseExpr0() {
    if(check(TokenType::NUMBER)) return parseNumLiteral();
    else return parseVarExpr();
}

std::unique_ptr<Program> Parser::Parse() {
    return parseProgram();
}
