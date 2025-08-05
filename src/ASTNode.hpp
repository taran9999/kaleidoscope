#include <vector>
#include <memory>
#include <string>

class Visitor;
class Program;
class FuncDef;
class Block;
class Expr;
class VarExpr;
class NumLiteral;
class BinOp;
class IfExpr;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(Visitor& v) = 0;
};

class Visitor {
    virtual ~Visitor() = default;
    virtual void visit(Program& node) = 0;
};

class Program : ASTNode {
public:
    std::vector<std::unique_ptr<FuncDef>> func_defs;

    explicit Program(std::vector<std::unique_ptr<FuncDef>> func_defs)
        : func_defs(std::move(func_defs)) {}

    void accept(Visitor& v) override;
};

class FuncDef : ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<VarExpr>> params;
    std::unique_ptr<Block> block;

    FuncDef(std::string name,
            std::vector<std::unique_ptr<VarExpr>> params,
            std::unique_ptr<Block> block)
        : name(std::move(name)), params(std::move(params)), block(std::move(block)) {}

    void accept(Visitor& v) override;
};

class Block : ASTNode {
public:
    std::vector<std::unique_ptr<Expr>> exprs;

    explicit Block(std::vector<std::unique_ptr<Expr>> exprs)
        : exprs(std::move(exprs)) {}

    void accept(Visitor& v) override;
};

class Expr : ASTNode {};

class VarExpr : Expr {
public:
    std::string name;

    explicit VarExpr(std::string name)
        : name(std::move(name)) {}

    void accept(Visitor& v) override;
};

class NumLiteral : Expr {
public:
    int val;

    explicit NumLiteral(int val)
        : val(val) {}

    void accept(Visitor& v) override;
};

class BinOp : Expr {
    std::unique_ptr<Expr> left;
    char op;  // maybe make an enum of ops
    std::unique_ptr<Expr> right;

    BinOp(std::unique_ptr<Expr> left,
          char op,
          std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(Visitor &v) override;
};

class IfExpr : Expr {
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Block> then;
    std::unique_ptr<Block> elss;

    IfExpr(std::unique_ptr<Expr> cond,
           std::unique_ptr<Block> then,
           std::unique_ptr<Block> elss)
        : cond(std::move(cond)), then(std::move(then)), elss(std::move(elss)) {}

    void accept(Visitor& v) override;
};
