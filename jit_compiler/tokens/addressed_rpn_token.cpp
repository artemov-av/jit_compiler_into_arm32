#include "addressed_rpn_token.h"

AddressedRPNToken::AddressedRPNToken(void *ptr) : _ptr(ptr) {
}

void *AddressedRPNToken::GetAddress() const {
    return _ptr;
}
