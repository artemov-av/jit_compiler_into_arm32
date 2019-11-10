#pragma once
#include "rpn_token.h"

class MinusRPNToken : public RPNToken {
public:
    TokenType GetTypeOfToken() const override;
};
