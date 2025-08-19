#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Parser.hpp"
#include "ASTNode.hpp"
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
        check(TokenType::IDENTIFIER) ||
        check(TokenType::LOOP);
}

bool Parser::at_end() {
    return pos >= tokens.size() - 1;
}

void Parser::advance() {
    if(!at_end()) pos++;
}

void Parser::error(TokenType expected) {
    Token curr = current();
    std::cerr << "Got " << string_of_token_type(curr.type) << " at " << curr.line << ":" << curr.col << " (expected " << string_of_token_type(expected) << ")" << std::endl;
    num_errors++;
}

void Parser::errorMultiple(std::vector<TokenType> expected) {
    Token curr = current();
    std::cerr << "Got " << string_of_token_type(curr.type) << " at " << curr.line << ":" << curr.col << " (expected ";

    for(size_t i = 0; i < expected.size(); i++) {
        if(i > 0) std::cerr << ", ";
        std::cerr << string_of_token_type(expected[i]);
    }
    std::cerr << std::endl;
    num_errors++;
}

void Parser::endProgError() {
    std::cerr << "Reached end of file while parsing" << std::endl;
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
    auto name = nameToken.data;

    std::vector<std::string> params;
    while(check(TokenType::IDENTIFIER)) {
        Token curr = current();
        params.push_back(curr.data);
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
    else if(check(TokenType::LOOP)) return parseLoopExpr();
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
    Token curr = accept(TokenType::IDENTIFIER);
    auto name = curr.data;
    return std::make_unique<VarExpr>(std::move(name));
}

std::unique_ptr<CallExpr> Parser::parseCallExpr() {
    std::vector<std::unique_ptr<Expr>> args;

    Token name_tok = accept(TokenType::IDENTIFIER);
    auto name = name_tok.data;

    accept(TokenType::LPAR);
    while(!check(TokenType::RPAR)) {
        if(current().type == TokenType::END_PROG) {
            endProgError();
            break;
        }

        auto arg = parseExpr();
        args.push_back(std::move(arg));
    }
    accept(TokenType::RPAR);

    return std::make_unique<CallExpr>(std::move(name), std::move(args));
}

std::unique_ptr<NumLiteral> Parser::parseNumLiteral() {
    Token curr = accept(TokenType::NUMBER);
    std::string data = curr.data;
    auto val = std::stoi(data);
    return std::make_unique<NumLiteral>(val);
}

std::unique_ptr<LoopExpr> Parser::parseLoopExpr() {
    accept(TokenType::LOOP);

    Token nameToken = accept(TokenType::IDENTIFIER);
    auto name = nameToken.data;

    accept(TokenType::RANGE);
    auto start = parseExpr();
    accept(TokenType::COMMA);
    auto end = parseExpr();
    accept(TokenType::COMMA);
    auto step = parseExpr();
    accept(TokenType::ARROW);
    auto block = parseBlock();
    accept(TokenType::END);

    return std::make_unique<LoopExpr>(std::move(name), std::move(start), std::move(end), std::move(step), std::move(block));
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

    while(check(TokenType::MINUS) || check(TokenType::PLUS)) {
        if(check(TokenType::MINUS)) {
            advance();
            auto rhs = parseExpr0();
            lhs = std::make_unique<BinOp>(std::move(lhs), '-', std::move(rhs));
        } else if(check(TokenType::PLUS)) {
            advance();
            auto rhs = parseExpr0();
            lhs = std::make_unique<BinOp>(std::move(lhs), '+', std::move(rhs));
        } else {
            // shouldn't happen
            errorMultiple({TokenType::MINUS, TokenType::PLUS});
        }
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::parseExpr0() {
    if(check(TokenType::NUMBER)) return parseNumLiteral();
    else if(check(TokenType::IDENTIFIER)) {
        if(lookahead(1).type == TokenType::LPAR) return parseCallExpr();
        else return parseVarExpr();
    }

    errorMultiple({TokenType::IF, TokenType::IDENTIFIER, TokenType::NUMBER});
    return std::make_unique<VarExpr>("err");
}

std::unique_ptr<ASTNode> Parser::Parse(bool toplevel) {
    if (!toplevel) return parseProgram();

    if(check(TokenType::DEF)) return parseFuncDef();
    else if(checkExpr()) {
        // make anonymous funcdef from toplevel expr
        auto e = parseExpr();
        std::vector<std::unique_ptr<Expr>> es;
        std::vector<std::string> p;
        es.push_back(std::move(e));
        auto b = std::make_unique<Block>(std::move(es));

        return std::make_unique<FuncDef>("_expr", std::move(p), std::move(b));
    }
    else return nullptr;
}

int Parser::Errors() {
    return num_errors;
}
