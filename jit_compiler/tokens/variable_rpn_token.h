#pragma once

#include "addressed_rpn_token.h"

class VariableRPNToken : public AddressedRPNToken {
public:
    explicit VariableRPNToken(void *ptr);

    TokenType GetTypeOfToken() const override;
};
