#pragma once

#include <vector>
#include <string>

class Tokenizer {
public:
    std::vector<std::string>
    operator()(const std::string &expr, char delim = ' ') const;
};
