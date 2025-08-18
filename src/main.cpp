#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "Token.hpp"
#include "Parser.hpp"
#include "PrintVisitor.hpp"
#include "LLVMGen.hpp"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        // Top level
        std::string line;
        LLVMGen gen;

        std::cout << "> ";
        while(std::getline(std::cin, line)) {
            std::istringstream iss(line);
            Lexer lexer(iss);
            std::vector<Token> tokens;
            Token token = lexer.NextToken();
            tokens.push_back(token);
            while(token.type != TokenType::END_PROG) {
                token = lexer.NextToken();
                tokens.push_back(token);
            }

            Parser parser(std::move(tokens));
            auto root = parser.Parse(true);
            if(!root) {
                std::cerr << "parsing error" << std::endl;
                return 1;
            }

            root->accept(gen);
            gen.PrintRes();

            std::cout << "> ";
        }

        gen.mod->print(llvm::outs(), nullptr);
        return 0;
    }

    std::ifstream f;
    f.open(argv[1]);
    if(!f.is_open()) {
        std::cout << "Unable to open file: " << argv[1] << std::endl;
        return 1;
    }

    std::vector<Token> tokens;
    Lexer lexer(f);
    Token token = lexer.NextToken();
    tokens.push_back(token);
    while(token.type != TokenType::END_PROG) {
        // std::cout << token.to_string() << "\n";
        token = lexer.NextToken();
        tokens.push_back(token);
    }
    f.close();

    Parser parser(std::move(tokens));
    auto root = parser.Parse();

    // PrintVisitor printer;
    // printer.visit(*root);

    LLVMGen gen;
    // gen.visit(*root);
    root->accept(gen);
    gen.mod->print(llvm::outs(), nullptr);
}

// // test mlir
// #include "mlirtest.cpp"
// int main() {
//     MLIRTest m;
//     m.gen42();
//     m.dump();
// }
