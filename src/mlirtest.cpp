#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include <llvm/Support/raw_ostream.h>

class MLIRTest {
    mlir::MLIRContext context;
    mlir::OpBuilder builder;
    mlir::ModuleOp module;

public:
    MLIRTest() : builder(&context) {
        context.loadDialect<mlir::arith::ArithDialect>();
        context.loadDialect<mlir::func::FuncDialect>();

        module = mlir::ModuleOp::create(builder.getUnknownLoc());
        builder.setInsertionPointToEnd(module.getBody());
    }

    void gen42() {
        auto funcType = builder.getFunctionType({}, {builder.getI32Type()});
        auto func = builder.create<mlir::func::FuncOp>(
            builder.getUnknownLoc(), "main", funcType
        );
        func.addEntryBlock();

        auto& entry = func.front();
        builder.setInsertionPointToStart(&entry);

        auto constant = builder.create<mlir::arith::ConstantOp>(
            builder.getUnknownLoc(),
            builder.getI32IntegerAttr(42)
        );

        builder.create<mlir::func::ReturnOp>(
            builder.getUnknownLoc(), constant.getResult()
        );
    }

    void dump() {
        module.print(llvm::outs());
    }

};
