#include "function_rpn_token.h"

FunctionRPNToken::FunctionRPNToken(void *ptr, int args_cnt)
        : AddressedRPNToken(ptr), _args_cnt(args_cnt) {
}

TokenType FunctionRPNToken::GetTypeOfToken() const {
    return FUNCTION;
}

int FunctionRPNToken::GetArgsCount() const {
    return _args_cnt;
}
