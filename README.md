# kaleidoscope
Kaleidoscope LLVM compiler frontend based on the tutorial.

This code differs from the tutorial slightly.
- I use a proper visitor pattern for codegen instead of virtual methods in the AST nodes. An AST Printer is also implemented through the visitor.
- The syntax differs slightly. I originally wanted to make a custom language while using the tutorial as a reference, but instead decided to implement the features from the tutorial as they are simple and fundamental features.
- Local variables don't define their own scope, instead they are local to the function they are defined in.
- The JIT from part 6 is not implemented, since it uses the existing Kaleidoscope JIT framework. I wanted to just make it my own, but I haven't done this yet.
- In the future, I want to integrate MLIR passes into the compiler. This might replace the current code generation strategy which directly generates LLVM IR from the AST.
