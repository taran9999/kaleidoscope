#include <iostream>

#include "PrintVisitor.hpp"

void PrintVisitor::visit(Program& node) {
    std::cout << "Program\n";
    for(const auto& fd : node.func_defs) fd->accept(*this);
    std::cout << std::endl;
}

void PrintVisitor::visit(FuncDef& node) {
    std::cout << "FuncDef " << node.name;

    for(const auto& p : node.params) std::cout << " " << p;
    std::cout << "\n";

    node.block->accept(*this);
}

void PrintVisitor::visit(Block& node) {
    std::cout << "Block\n";
    for(const auto& e : node.exprs) e->accept(*this);
}

void PrintVisitor::visit(VarExpr& node) {
    std::cout << "VarExpr(" << node.name << ")\n";
}

void PrintVisitor::visit(NumLiteral& node) {
    std::cout << "NumLiteral(" << node.val << ")\n";
}

void PrintVisitor::visit(BinOp& node) {
    std::cout << "BinOp\n";
    node.left->accept(*this);
    std::cout << node.op << "\n";
    node.right->accept(*this);
}

void PrintVisitor::visit(IfExpr& node) {
    std::cout << "If\n";
    node.cond->accept(*this);
    node.then->accept(*this);
    node.elss->accept(*this);
}
