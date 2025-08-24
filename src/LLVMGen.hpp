#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include <llvm/IR/Instructions.h>
#include <memory>
#include <string>
#include <map>

#include "ASTNode.hpp"

class LLVMGen : public Visitor {
private:
    llvm::Value* res;
    llvm::AllocaInst* resAddr;
    bool fail;

    void error(std::string message);

    llvm::AllocaInst* allocLocalVarInFunc(llvm::Function* func, llvm::StringRef varName);

public:
    LLVMGen() : res(nullptr), resAddr(nullptr), fail(false) {
        ctx = std::make_unique<llvm::LLVMContext>();
        mod = std::make_unique<llvm::Module>("kl", *ctx);
        builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
    }

    std::unique_ptr<llvm::LLVMContext> ctx;
    std::unique_ptr<llvm::Module> mod;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::map<std::string, llvm::AllocaInst*> env;

    void visit(Program& node) override;
    void visit(FuncDef& node) override;
    void visit(Block& node) override;
    void visit(Extern& node) override;
    void visit(VarExpr& node) override;
    void visit(NumLiteral& node) override;
    void visit(BinOp& node) override;
    void visit(IfExpr& node) override;
    void visit(CallExpr& node) override;
    void visit(LoopExpr& node) override;
    void visit(VarInitExpr& node) override;
    void visit(AssignExpr& node) override;

    void PrintRes();
    void EmitObject();
};
