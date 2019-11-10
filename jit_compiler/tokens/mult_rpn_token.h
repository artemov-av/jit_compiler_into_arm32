#pragma once
#include "rpn_token.h"


class MultRPNToken : public RPNToken {
public:
    TokenType GetTypeOfToken() const override;
};
