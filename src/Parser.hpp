#include <cstddef>
#include <vector>
#include <memory>

#include "Token.hpp"
#include "ASTNode.hpp"

class Parser {
private:
    std::vector<Token> tokens;
    std::size_t pos;
    Token end_token;  // dummy end token to use as current token if pos exceeds the size of tokens vector
    int num_errors;

    // utility functions
    Token current();
    Token lookahead(std::size_t ofs = 1);
    Token accept(TokenType expected);

    bool check(TokenType expected);
    bool checkExpr();
    bool at_end();

    void advance();
    void error(TokenType expected);
    void errorMultiple(std::vector<TokenType> expected);
    void endProgError();

    std::unique_ptr<Program> parseProgram();
    std::unique_ptr<FuncDef> parseFuncDef();
    std::unique_ptr<Block> parseBlock();
    std::unique_ptr<Extern> parseExtern();

    std::unique_ptr<Expr> parseExpr();
    std::unique_ptr<Expr> parseExpr0();
    std::unique_ptr<Expr> parseExpr1();
    std::unique_ptr<Expr> parseExpr2();
    std::unique_ptr<Expr> parseExpr3();

    std::unique_ptr<VarExpr> parseVarExpr();
    std::unique_ptr<CallExpr> parseCallExpr();
    std::unique_ptr<NumLiteral> parseNumLiteral();
    std::unique_ptr<IfExpr> parseIfExpr();
    std::unique_ptr<LoopExpr> parseLoopExpr();
    std::unique_ptr<VarInitExpr> parseVarInitExpr();

public:
    explicit Parser(std::vector<Token> tokens)
        : tokens(std::move(tokens)), pos(0), end_token(Token(TokenType::END_PROG, "", 0, 0)), num_errors(0) {}

    std::unique_ptr<ASTNode> Parse(bool toplevel = false);
    int Errors();
};
