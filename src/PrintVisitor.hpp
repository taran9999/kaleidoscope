#include "ASTNode.hpp"

class PrintVisitor : Visitor {
public:
    void visit(Program& node) override;
    void visit(FuncDef& node) override;
    void visit(Block& node) override;
    void visit(VarExpr& node) override;
    void visit(NumLiteral& node) override;
    void visit(BinOp& node) override;
    void visit(IfExpr& node) override;
};
