#pragma once
#include "rpn_token.h"

class LiteralRPNToken : public RPNToken {
public:
    explicit LiteralRPNToken(int value);

    TokenType GetTypeOfToken() const override;

    int GetValue() const;

protected:
    int _value;
};
