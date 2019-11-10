#pragma once

#include "rpn_token.h"

class AddressedRPNToken : public RPNToken {
public:
    AddressedRPNToken() = default;

    explicit AddressedRPNToken(void *ptr);

    TokenType GetTypeOfToken() const override = 0;

    void *GetAddress() const;

protected:
    void *_ptr;
};
