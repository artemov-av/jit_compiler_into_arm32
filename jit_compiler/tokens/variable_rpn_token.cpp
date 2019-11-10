#include "variable_rpn_token.h"

VariableRPNToken::VariableRPNToken(void *ptr) : AddressedRPNToken(ptr) {
}

TokenType VariableRPNToken::GetTypeOfToken() const {
    return VARIABLE;
}
