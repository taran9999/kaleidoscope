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
public:
    virtual ~Visitor() = default;
    virtual void visit(Program& node) = 0;
    virtual void visit(FuncDef& node) = 0;
    virtual void visit(Block& node) = 0;
    virtual void visit(Expr& node) = 0;
    virtual void visit(VarExpr& node) = 0;
    virtual void visit(NumLiteral& node) = 0;
    virtual void visit(BinOp& node) = 0;
    virtual void visit(IfExpr& node) = 0;
};

template<typename Derived>
class Visitable : public ASTNode {
public:
    void accept(Visitor& v) override {
        v.visit(static_cast<Derived&>(*this));
    }
};

class Program : public Visitable<Program> {
public:
    std::vector<std::unique_ptr<FuncDef>> func_defs;

    explicit Program(std::vector<std::unique_ptr<FuncDef>> func_defs)
        : func_defs(std::move(func_defs)) {}

    void accept(Visitor& v) override;
};

class FuncDef : public Visitable<FuncDef>{
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

class Block : public Visitable<Block> {
public:
    std::vector<std::unique_ptr<Expr>> exprs;

    explicit Block(std::vector<std::unique_ptr<Expr>> exprs)
        : exprs(std::move(exprs)) {}

    void accept(Visitor& v) override;
};

class Expr : public ASTNode {};

class VarExpr : Expr {
public:
    std::string name;

    explicit VarExpr(std::string name)
        : name(std::move(name)) {}

    void accept(Visitor& v) override;
};

class NumLiteral : public Expr, public Visitable<NumLiteral> {
public:
    int val;

    explicit NumLiteral(int val)
        : val(val) {}

    void accept(Visitor& v) override;
};

class BinOp : public Expr, public Visitable<BinOp> {
    std::unique_ptr<Expr> left;
    char op;  // maybe make an enum of ops
    std::unique_ptr<Expr> right;

    BinOp(std::unique_ptr<Expr> left,
          char op,
          std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(Visitor &v) override;
};

class IfExpr : public Expr, public Visitable<IfExpr> {
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Block> then;
    std::unique_ptr<Block> elss;

    IfExpr(std::unique_ptr<Expr> cond,
           std::unique_ptr<Block> then,
           std::unique_ptr<Block> elss)
        : cond(std::move(cond)), then(std::move(then)), elss(std::move(elss)) {}

    void accept(Visitor& v) override;
};
