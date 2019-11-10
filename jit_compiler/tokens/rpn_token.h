#pragma once

enum TokenType {
    VARIABLE, LITERAL, FUNCTION, OP_PLUS, OP_MINUS, OP_MULT
};

class RPNToken {
public:
    virtual ~RPNToken() = default;

    virtual TokenType GetTypeOfToken() const = 0;
};