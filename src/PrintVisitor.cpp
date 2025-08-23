#include <iostream>
#include <string>

#include "PrintVisitor.hpp"
#include "ASTNode.hpp"

void PrintVisitor::visit(Program& node) {
    std::cout << "Program\n";

    for(const auto& e : node.externs) {
        indent_level = 1;
        e->accept(*this);
    }

    for(const auto& fd : node.func_defs) {
        indent_level = 1;
        fd->accept(*this);
    }

    std::cout << std::endl;
}

void PrintVisitor::visit(FuncDef& node) {
    unsigned int curr_indent = indent_level;
    print_indent(indent_level);

    std::cout << "FuncDef " << node.name;

    for(const auto& p : node.params) std::cout << " " << p;
    std::cout << "\n";

    indent_level++;
    node.block->accept(*this);
    indent_level = curr_indent;
}

void PrintVisitor::visit(Block& node) {
    unsigned int curr_indent = indent_level;
    print_indent(indent_level);

    std::cout << "Block\n";
    for(const auto& e : node.exprs) {
        indent_level = curr_indent + 1;
        e->accept(*this);
    }

    indent_level = curr_indent;
}

void PrintVisitor::visit(Extern& node) {
    print_indent(indent_level);
    std::cout << "Extern " << node.name;
    for(const auto& p : node.params) std::cout << " " << p;
    std::cout << "\n";
}

void PrintVisitor::visit(VarExpr& node) {
    print_indent(indent_level);
    std::cout << "VarExpr(" << node.name << ")\n";
}

void PrintVisitor::visit(NumLiteral& node) {
    print_indent(indent_level);
    std::cout << "NumLiteral(" << node.val << ")\n";
}

void PrintVisitor::visit(BinOp& node) {
    unsigned int curr_indent = indent_level;
    print_indent(indent_level);
    std::cout << "BinOp\n";

    indent_level = curr_indent + 1;
    node.left->accept(*this);

    print_indent(indent_level + 1);
    std::cout << node.op << "\n";

    indent_level = curr_indent + 1;
    node.right->accept(*this);

    indent_level = curr_indent;
}

void PrintVisitor::visit(IfExpr& node) {
    unsigned int curr_indent = indent_level;
    print_indent(indent_level);
    std::cout << "If\n";

    indent_level = curr_indent + 1;
    node.cond->accept(*this);

    indent_level = curr_indent + 1;
    node.then->accept(*this);

    indent_level = curr_indent + 1;
    node.elss->accept(*this);

    indent_level = curr_indent;
}

void PrintVisitor::visit(CallExpr& node) {
    unsigned int curr_indent = indent_level;
    print_indent(indent_level);
    std::cout << "Call " << node.name << "\n";

    for(const auto& arg : node.args) {
        indent_level = curr_indent + 1;
        arg->accept(*this);
    }
}

void PrintVisitor::visit(LoopExpr& node) {
    unsigned int curr_indent = indent_level;
    print_indent(indent_level);
    std::cout << "Loop " << node.name << "\n";

    indent_level = curr_indent + 1;
    node.rangeStart->accept(*this);

    indent_level = curr_indent + 1;
    node.rangeEnd->accept(*this);

    indent_level = curr_indent + 1;
    node.step->accept(*this);

    indent_level = curr_indent + 1;
    node.block->accept(*this);

    indent_level = curr_indent;
}

void PrintVisitor::visit(VarInitExpr& node) {
    unsigned int curr_indent = indent_level;
    print_indent(indent_level);
    std::cout << "VarInit " << node.name << "\n";

    indent_level = curr_indent + 1;
    node.val->accept(*this);

    indent_level = curr_indent;
}

void PrintVisitor::visit(AssignExpr& node) {
    unsigned int curr_indent = indent_level;
    print_indent(indent_level);
    std::cout << "Assign\n";

    indent_level = curr_indent + 1;
    node.lhs->accept(*this);

    indent_level = curr_indent + 1;
    node.val->accept(*this);

    indent_level = curr_indent;
}

void PrintVisitor::print_indent(unsigned int level) {
    std::cout << std::string(level * 2, ' ');
}
