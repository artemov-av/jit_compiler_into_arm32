#include <stack>
#include "symbol.h"
#include "tokens/function_rpn_token.h"
#include "tokens/variable_rpn_token.h"
#include "tokens/addressed_rpn_token.h"
#include "tokens/mult_rpn_token.h"
#include "tokens/minus_rpn_token.h"
#include "tokens/plus_rpn_token.h"
#include "tokens/literal_rpn_token.h"
#include "translator_into_rpn.h"

std::vector<std::shared_ptr<RPNToken>>
TranslatorIntoRPN::operator()(const std::vector<std::string> &tokens, const std::vector<Symbol> &externs) const {
    auto address_map = BuildAddressMap(externs);

    std::vector<std::shared_ptr<RPNToken>> translated;
    std::stack<std::string> stack;
    std::vector<int> args_cnt_stack;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i][0] == '(') {
            stack.push(tokens[i]);
        } else if (IsOperator(tokens[i])) {
            while (!stack.empty()) {
                auto ch = stack.top()[0];
                if (!IsOperator(stack.top())) {
                    break;
                }
                if (GetPriorityOfOperator(ch) <
                    GetPriorityOfOperator(tokens[i][0])) {
                    break;
                }
                stack.pop();
                PushOperatorToken(translated, ch);
            }
            stack.push(tokens[i]);
        } else if (
                tokens[i][0] >= '0' && tokens[i][0] <= '9' ||
                tokens[i][0] == '-' && tokens[i][1] >= '0' &&
                tokens[i][1] <= '9') {
            translated.emplace_back(
                    std::make_shared<LiteralRPNToken>(std::stoi(tokens[i])));
        } else if (tokens[i][0] == ',') {
            ++args_cnt_stack[args_cnt_stack.size() - 1];
            while (!stack.empty() && stack.top()[0] != '(') {
                auto ch = stack.top()[0];
                stack.pop();
                PushOperatorToken(translated, ch);
            }
        } else if (tokens[i][0] == ')') {
            while (!stack.empty() && stack.top()[0] != '(') {
                auto ch = stack.top()[0];
                stack.pop();
                PushOperatorToken(translated, ch);
            }
            stack.pop();
            if (!stack.empty() && !IsOperator(stack.top()) &&
                stack.top()[0] != '(') {
                auto args_cnt = args_cnt_stack[args_cnt_stack.size() - 1];
                translated.emplace_back(std::make_shared<FunctionRPNToken>(
                        address_map[stack.top()], args_cnt));
                args_cnt_stack.pop_back();
                stack.pop();
            }
        } else {
            if (i + 1 < tokens.size() && tokens[i + 1][0] == '(') {
                stack.push(tokens[i]);
                if (tokens[i + 2][0] == ')') {
                    args_cnt_stack.push_back(0);
                } else {
                    args_cnt_stack.push_back(1);
                }
            } else {
                translated.emplace_back(std::make_shared<VariableRPNToken>(
                        address_map[tokens[i]]));
            }
        }
    }
    while (!stack.empty()) {
        char ch = stack.top()[0];
        stack.pop();
        PushOperatorToken(translated, ch);
    }

    return translated;
}

std::unordered_map<std::string, void *> TranslatorIntoRPN::BuildAddressMap(const std::vector<Symbol> &externs) const {
    std::unordered_map<std::string, void *> address_map;
    for (const auto &symbol : externs) {
        address_map.insert(std::make_pair(symbol.name, symbol.pointer));
    }
    return address_map;
}

bool TranslatorIntoRPN::IsOperator(const std::string &str) const {
    if (str.length() != 1) {
        return false;
    }
    return str[0] == '+' || str[0] == '-' || str[0] == '*';
}

int TranslatorIntoRPN::GetPriorityOfOperator(char op) const {
    switch (op) {
        case '+':
            return 0;
        case '-':
            return 0;
        case '*':
            return 1;
        default:
            return -1;
    }
}

void TranslatorIntoRPN::PushOperatorToken(std::vector<std::shared_ptr<RPNToken>> &vec, char ch) const {
    switch (ch) {
        case '+':
            vec.emplace_back(std::make_shared<PlusRPNToken>());
            break;
        case '-':
            vec.emplace_back(std::make_shared<MinusRPNToken>());
            break;
        case '*':
            vec.emplace_back(std::make_shared<MultRPNToken>());
            break;
    }
}
