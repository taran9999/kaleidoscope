#include "Token.hpp"

#define X(name) case TokenType::name: return #name;
const char* string_of_token_type(TokenType type) {
    switch(type) { TOKEN_TYPES }
}
#undef X
