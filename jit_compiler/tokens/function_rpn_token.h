#pragma once

#include "addressed_rpn_token.h"


class FunctionRPNToken : public AddressedRPNToken {
public:
    FunctionRPNToken(void *ptr, int args_cnt);

    TokenType GetTypeOfToken() const override;

    int GetArgsCount() const;

protected:
    int _args_cnt;
};
