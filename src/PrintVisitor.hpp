#include "ASTNode.hpp"

class PrintVisitor : public Visitor {
private:
    unsigned int indent_level;

    void print_indent(unsigned int level);
public:
    PrintVisitor()
        : indent_level(0) {}

    void visit(Program& node) override;
    void visit(FuncDef& node) override;
    void visit(Block& node) override;
    void visit(VarExpr& node) override;
    void visit(NumLiteral& node) override;
    void visit(BinOp& node) override;
    void visit(IfExpr& node) override;
    void visit(CallExpr& node) override;
    void visit(LoopExpr& node) override;
    void visit(VarInitExpr& node) override;
    void visit(AssignExpr& node) override;
};
