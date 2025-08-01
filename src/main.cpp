#include <iostream>
#include <fstream>
#include <vector>

#include "Lexer.hpp"
#include "Token.hpp"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Input file required" << std::endl;
        return 1;
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
    while(token.type != TokenType::END_PROG) {
        std::cout << token.to_string() << "\n";
        token = lexer.NextToken();
    }
}
