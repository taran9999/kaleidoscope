#ifndef ASTNODE_HPP
#define ASTNODE_HPP

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
class CallExpr;
class LoopExpr;

class ASTNode {
public:
    virtual ~ASTNode();
    virtual void accept(Visitor& v) = 0;
};

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(Program& node) = 0;
    virtual void visit(FuncDef& node) = 0;
    virtual void visit(Block& node) = 0;
    virtual void visit(VarExpr& node) = 0;
    virtual void visit(NumLiteral& node) = 0;
    virtual void visit(BinOp& node) = 0;
    virtual void visit(IfExpr& node) = 0;
    virtual void visit(CallExpr& node) = 0;
    virtual void visit(LoopExpr& node) = 0;
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
};

class FuncDef : public Visitable<FuncDef>{
public:
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<Block> block;

    FuncDef(std::string name,
            std::vector<std::string> params,
            std::unique_ptr<Block> block)
        : name(std::move(name)), params(std::move(params)), block(std::move(block)) {}
};

class Block : public Visitable<Block> {
public:
    std::vector<std::unique_ptr<Expr>> exprs;

    explicit Block(std::vector<std::unique_ptr<Expr>> exprs)
        : exprs(std::move(exprs)) {}
};

class Expr : public ASTNode {};

class VarExpr : public Expr, public Visitable<VarExpr> {
public:
    std::string name;

    explicit VarExpr(std::string name)
        : name(std::move(name)) {}

    // Expr still has the purely virtual accept method from ASTNode. We want the one that Visitable overrides.
    void accept(Visitor& v) override {
        Visitable<VarExpr>::accept(v);
    }
};

class NumLiteral : public Expr, public Visitable<NumLiteral> {
public:
    int val;

    explicit NumLiteral(int val)
        : val(val) {}

    void accept(Visitor& v) override {
        Visitable<NumLiteral>::accept(v);
    }
};

class BinOp : public Expr, public Visitable<BinOp> {
public:
    std::unique_ptr<Expr> left;
    char op;  // maybe make an enum of ops
    std::unique_ptr<Expr> right;

    BinOp(std::unique_ptr<Expr> left,
          char op,
          std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(Visitor& v) override {
        Visitable<BinOp>::accept(v);
    }
};

class IfExpr : public Expr, public Visitable<IfExpr> {
public:
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Block> then;
    std::unique_ptr<Block> elss;

    IfExpr(std::unique_ptr<Expr> cond,
           std::unique_ptr<Block> then,
           std::unique_ptr<Block> elss)
        : cond(std::move(cond)), then(std::move(then)), elss(std::move(elss)) {}

    void accept(Visitor& v) override {
        Visitable<IfExpr>::accept(v);
    }
};

class CallExpr : public Expr, public Visitable<CallExpr> {
public:
    std::string name;
    std::vector<std::unique_ptr<Expr>> args;

    CallExpr(std::string name,
             std::vector<std::unique_ptr<Expr>> args)
        : name(std::move(name)), args(std::move(args)) {}

    void accept(Visitor& v) override {
        Visitable<CallExpr>::accept(v);
    }
};

class LoopExpr : public Expr, public Visitable<LoopExpr> {
public:
    std::string name;
    std::unique_ptr<Expr> rangeStart;
    std::unique_ptr<Expr> rangeEnd;
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Expr> step;

    LoopExpr(std::string name,
             std::unique_ptr<Expr> rangeStart,
             std::unique_ptr<Expr> rangeEnd,
             std::unique_ptr<Expr> cond,
             std::unique_ptr<Expr> step)
        : name(std::move(name)), rangeStart(std::move(rangeStart)), rangeEnd(std::move(rangeEnd)), cond(std::move(cond)), step(std::move(step)) {}

    void accept(Visitor& v) override {
        Visitable<LoopExpr>::accept(v);
    }
};

#endif
