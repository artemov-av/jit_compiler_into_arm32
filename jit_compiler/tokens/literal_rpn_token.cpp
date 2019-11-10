#include "literal_rpn_token.h"

LiteralRPNToken::LiteralRPNToken(int value) : _value(value) {
}

TokenType LiteralRPNToken::GetTypeOfToken() const {
    return LITERAL;
}

int LiteralRPNToken::GetValue() const {
    return _value;
}
