#pragma once

#include "tokens/rpn_token.h"
#include "symbol.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

class TranslatorIntoRPN {
public:
    std::vector<std::shared_ptr<RPNToken>> operator()(
            const std::vector<std::string> &tokens,
            const std::vector<Symbol> &externs) const;

private:
    std::unordered_map<std::string, void *>
    BuildAddressMap(const std::vector<Symbol> &externs) const;

    bool IsOperator(const std::string &str) const;

    int GetPriorityOfOperator(char op) const;

    void PushOperatorToken(std::vector<std::shared_ptr<RPNToken>> &vec, char ch)
    const;
};
