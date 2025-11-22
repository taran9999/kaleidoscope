#include "LLVMGen.hpp"
#include "ASTNode.hpp"
#include <iostream>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>
#include <system_error>
#include <vector>

void LLVMGen::visit(Program& node) {
    for(const auto& e : node.externs) {
        e->accept(*this);
        if(!res) {
            error("prog gen failed");
            return;
        }
    }

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

    llvm::BasicBlock* bb = llvm::BasicBlock::Create(*ctx, "entry", f);
    builder->SetInsertPoint(bb);

    env.clear();
    for(auto& arg : f->args()) {
        auto* alloc = allocLocalVarInFunc(f, arg.getName());
        builder->CreateStore(&arg, alloc);
        env[std::string(arg.getName())] = alloc;
    }

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

void LLVMGen::visit(Extern& node) {
    // create the function without writing the body
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

    res = f;
}

void LLVMGen::visit(VarExpr& node) {
    auto* a = env[node.name];
    if(!a) {
        error("unbound variable: " + node.name);
    }

    res = builder->CreateLoad(a->getAllocatedType(), a, node.name.c_str());
    resAddr = a;
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

void LLVMGen::visit(LoopExpr& node) {
    node.rangeStart->accept(*this);
    if(!res) {
        error("failed to generate code of loop start val");
        return;
    }
    llvm::Value* start = res;

    node.rangeEnd->accept(*this);
    if(!res) {
        error("failed to generate code of loop end val");
        return;
    }
    llvm::Value* end = res;

    auto* currFunc = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopBlock = llvm::BasicBlock::Create(*ctx, "loop", currFunc);
    builder->CreateBr(loopBlock);

    builder->SetInsertPoint(loopBlock);
    auto* loopVar = allocLocalVarInFunc(currFunc, node.name);
    builder->CreateStore(start, loopVar);

    // loop var shadows and then restores original value
    llvm::AllocaInst* oldVarVal = env[node.name];
    env[node.name] = loopVar;

    node.block->accept(*this);
    if(!res) {
        error("failed to generate body of loop");
        return;
    }

    node.step->accept(*this);
    if(!res) {
        error("failed to generate code of loop step val");
        return;
    }
    llvm::Value* step = res;

    llvm::Value* loopVarLoaded = builder->CreateLoad(loopVar->getAllocatedType(), loopVar, "loopVarLoad");
    llvm::Value* nextVar = builder->CreateFAdd(loopVarLoaded, step, "nextLoopVar");
    llvm::Value* endCond = builder->CreateFCmpONE(nextVar, end, "loopEndCond");

    llvm::BasicBlock* postLoopBlock = llvm::BasicBlock::Create(*ctx, "endLoop", currFunc);
    builder->CreateCondBr(endCond, loopBlock, postLoopBlock);
    builder->SetInsertPoint(postLoopBlock);
    builder->CreateStore(nextVar, loopVar);

    if(oldVarVal) env[node.name] = oldVarVal;
    else env.erase(node.name);

    res = llvm::Constant::getNullValue(llvm::Type::getDoubleTy(*ctx));
}

void LLVMGen::visit(VarInitExpr& node) {
    auto* test = env[node.name];
    if(test) {
        error("redefined variable: " + node.name);
        res = nullptr;
        return;
    }

    auto* alloc = allocLocalVarInFunc(builder->GetInsertBlock()->getParent(), node.name);

    node.val->accept(*this);
    if(!res) {
        error("failed to codegen value of var init: " + node.name);
        return;
    }
    auto* val = res;

    builder->CreateStore(val, alloc);
    env[node.name] = alloc;
}

void LLVMGen::visit(AssignExpr& node) {
    node.val->accept(*this);
    if(!res) {
        error("failed to codegen rhs of assignment");
        return;
    }
    auto* val = res;

    node.lhs->accept(*this);
    if(!resAddr) {
        error("failed to get address of lhs of assign");
        res = nullptr;
        return;
    }

    auto* addr = resAddr;

    builder->CreateStore(val, addr);
    res = val;
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
    std::cout << std::endl;
}

void LLVMGen::EmitObject() {
    auto targetTripleStr = llvm::sys::getDefaultTargetTriple();
    auto targetTriple = llvm::Triple(targetTripleStr);

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string err;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, err);
    if(!target) {
        error(err);
        return;
    }

    auto cpu = "generic";
    auto features = "";
    llvm::TargetOptions opt;
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, llvm::Reloc::PIC_);

    mod->setDataLayout(targetMachine->createDataLayout());
    mod->setTargetTriple(targetTriple);

    auto fname = "out.o";
    std::error_code ec;
    llvm::raw_fd_ostream dest(fname, ec, llvm::sys::fs::OF_None);
    if(ec) {
        error("failed to open file: " + ec.message());
        return;
    }

    llvm::legacy::PassManager pm;
    auto ftype = llvm::CodeGenFileType::ObjectFile;
    if(targetMachine->addPassesToEmitFile(pm, dest, nullptr, ftype)) {
        error("targetMachine can't emit file of this type");
        return;
    }

    pm.run(*mod);
    dest.flush();
}

llvm::AllocaInst* LLVMGen::allocLocalVarInFunc(llvm::Function* func, llvm::StringRef varName) {
    llvm::IRBuilder<> b(&func->getEntryBlock(), func->getEntryBlock().begin());
    return b.CreateAlloca(llvm::Type::getDoubleTy(*ctx), nullptr, varName);
}
