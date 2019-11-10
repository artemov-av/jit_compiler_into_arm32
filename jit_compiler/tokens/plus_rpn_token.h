#pragma once
#include "rpn_token.h"

class PlusRPNToken : public RPNToken {
public:
    TokenType GetTypeOfToken() const override;
};
