#pragma once

#include <string>
#include <vector>
#include "translator_into_rpn.h"
#include "symbol.h"

class JITCompilerIntoARM32 {
public:
    void CompileIntoAllocatedMemory(
            const std::string &expression,
            const std::vector<Symbol> &externs,
            void *out_buffer);
};

extern "C" void jit_compile_expression_to_arm(
        const char *expression,
        const Symbol *externs,
        void *out_buffer);
