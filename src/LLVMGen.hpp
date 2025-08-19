#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include <memory>
#include <string>
#include <map>

#include "ASTNode.hpp"

class LLVMGen : public Visitor {
private:
    llvm::Value* res;

    void error(std::string message);

public:
    LLVMGen() : res(nullptr) {
        ctx = std::make_unique<llvm::LLVMContext>();
        mod = std::make_unique<llvm::Module>("kl", *ctx);
        builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
    }

    std::unique_ptr<llvm::LLVMContext> ctx;
    std::unique_ptr<llvm::Module> mod;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::map<std::string, llvm::Value*> env;

    void visit(Program& node) override;
    void visit(FuncDef& node) override;
    void visit(Block& node) override;
    void visit(VarExpr& node) override;
    void visit(NumLiteral& node) override;
    void visit(BinOp& node) override;
    void visit(IfExpr& node) override;
    void visit(CallExpr& node) override;
    void visit(LoopExpr& node) override;

    void PrintRes();
};
