#include "LLVMGen.hpp"
#include "ASTNode.hpp"
#include <iostream>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <vector>

void LLVMGen::visit(Program& node) {
    for(const auto& fd : node.func_defs) {
        fd->accept(*this);
        if(!res) {
            error("prog gen failed");
            return;
        }
    }
}

// TOOD: add prototypes and function redefinition checking
void LLVMGen::visit(FuncDef& node) {
    std::vector<llvm::Type*> t(node.params.size(), llvm::Type::getDoubleTy(*ctx));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*ctx), t, false);
    llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, node.name, mod.get());
    if(!f) {
        error("failed to create function: " + node.name);
        res = nullptr;
        return;
    }

    size_t i = 0;
    for(auto& arg : f->args()) arg.setName(node.params[i++]);

    llvm::BasicBlock *bb = llvm::BasicBlock::Create(*ctx, "entry", f);
    builder->SetInsertPoint(bb);

    env.clear();
    for(auto& arg : f->args()) env[std::string(arg.getName())] = &arg;

    node.block->accept(*this);
    if(!res) {
        error("failed to generate body for function: " + node.name);
        f->eraseFromParent();
        return;
    }
    builder->CreateRet(res);

    llvm::verifyFunction(*f);
    res = f;
}


void LLVMGen::visit(Block& node) {
    // evaluate the block to the value of the last expression (left in res)

    for(auto& e : node.exprs) {
        e->accept(*this);
        if(!res) {
            error("block failed while evaluating expr");
            return;
        }
    }
}

void LLVMGen::visit(VarExpr& node) {
    res = env[node.name];
    if(!res) {
        error("unbound variable: " + node.name);
    }
}

// TODO: maybe change AST node from int val to float val
void LLVMGen::visit(NumLiteral& node) {
    res = llvm::ConstantFP::get(*ctx, llvm::APFloat(double(node.val)));
}

void LLVMGen::visit(BinOp& node) {
    node.left->accept(*this);
    if(!res) {
        error("failed to generate lhs of binop node");
        return;
    }
    llvm::Value* lhs = res;

    node.right->accept(*this);
    if(!res) {
        error("failed to generate rhs of binop node");
        return;
    }
    llvm::Value* rhs = res;

    switch(node.op) {
        case '-':
            res = builder->CreateFSub(lhs, rhs, "sub");
            return;
        case '+':
            res = builder->CreateFAdd(lhs, rhs, "add");
            return;
        case '<':
            res = builder->CreateFCmpULT(lhs, rhs, "lt");
            res = builder->CreateUIToFP(res, llvm::Type::getDoubleTy(*ctx), "bool");
            return;
        default:
            error("invalid binary operator");
            res = nullptr;
            return;
    }
}

void LLVMGen::visit(IfExpr& node) {
    node.cond->accept(*this);
    if(!res) {
        error("failed to generate code for if condition");
        return;
    }
    llvm::Value* cond = builder->CreateFCmpONE(res, llvm::ConstantFP::get(*ctx, llvm::APFloat(0.0)), "cond");

    llvm::Function* currFunc = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* then = llvm::BasicBlock::Create(*ctx, "then");
    llvm::BasicBlock* elss = llvm::BasicBlock::Create(*ctx, "else");
    llvm::BasicBlock* merge = llvm::BasicBlock::Create(*ctx, "merge");
    builder->CreateCondBr(cond, then, elss);

    currFunc->insert(currFunc->end(), then);
    builder->SetInsertPoint(then);
    node.then->accept(*this);
    if(!res) {
        error("failed to generate code for then block of if condition");
        return;
    }
    llvm::Value* thenVal = res;
    builder->CreateBr(merge);
    then = builder->GetInsertBlock();

    currFunc->insert(currFunc->end(), elss);
    builder->SetInsertPoint(elss);
    node.elss->accept(*this);
    if(!res) {
        error("failed to generate code for else block of if condition");
        return;
    }
    llvm::Value* elseVal = res;
    builder->CreateBr(merge);
    elss = builder->GetInsertBlock();

    currFunc->insert(currFunc->end(), merge);
    builder->SetInsertPoint(merge);
    llvm::PHINode* phi = builder->CreatePHI(llvm::Type::getDoubleTy(*ctx), 2, "phi");
    phi->addIncoming(thenVal, then);
    phi->addIncoming(elseVal, elss);

    res = phi;
}

void LLVMGen::visit(CallExpr& node) {
    auto* func = mod->getFunction(node.name);
    if(!func) {
        error("failed to find function " + node.name + " when generating calling code");
        res = nullptr;
        return;
    }

    if(node.args.size() != func->arg_size()) {
        error("function " + node.name + " called with wrong number of arguments");
        res = nullptr;
        return;
    }

    std::vector<llvm::Value*> argValues;
    for(size_t i = 0; i < node.args.size(); i++) {
        node.args[i]->accept(*this);
        if(!res) {
            error("failed codegen for for argument to funcall: " + node.name);
            return;
        }
        argValues.push_back(res);
    }

    res = builder->CreateCall(func, argValues, "call_" + node.name);
}

void LLVMGen::error(std::string message) {
    std::cerr << "LLVMGen: " << message << std::endl;
}

void LLVMGen::PrintRes() {
    if(!res) {
        std::cout << "current res is null" << std::endl;
        return;
    }

    res->print(llvm::outs());
}
