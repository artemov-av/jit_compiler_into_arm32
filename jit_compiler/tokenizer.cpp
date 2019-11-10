#include "tokenizer.h"

std::vector<std::string> Tokenizer::operator()(const std::string &expr, char delim) const {
    std::vector<std::string> tokens;
    std::string cur_token;
    std::string service_symbols = "(),+-*";
    bool fl = false;
    for (auto ch : expr) {
        if (ch == delim) {
            fl = true;
            continue;
        } else if (service_symbols.find(ch) != std::string::npos) {
            fl = false;
            if (ch == '-' && cur_token.empty() &&
                (tokens.empty() || tokens.back() != ")")) {
                cur_token.push_back(ch);
                continue;
            }
            if (!cur_token.empty()) {
                tokens.push_back(cur_token);
            }
            cur_token.clear();
            tokens.emplace_back(1, ch);
        } else {
            if(fl) {
                fl = false;
                if(!cur_token.empty()) {
                    tokens.push_back(cur_token);
                    cur_token.clear();
                }
            }
            cur_token.push_back(ch);
        }
    }
    if (!cur_token.empty()) {
        tokens.push_back(cur_token);
    }
    return tokens;
}
