#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "tokens/rpn_token.h"
#include "tokens/literal_rpn_token.h"
#include "tokens/plus_rpn_token.h"
#include "tokens/minus_rpn_token.h"
#include "tokens/mult_rpn_token.h"
#include "tokens/addressed_rpn_token.h"
#include "tokens/variable_rpn_token.h"
#include "tokens/function_rpn_token.h"
#include "tokenizer.h"
#include "translator_into_rpn.h"
#include "arm32_instructions_encoder.h"
#include "jit_compiler_into_arm32.h"
#include "symbol.h"

void
JITCompilerIntoARM32::CompileIntoAllocatedMemory(const std::string &expression, const std::vector <Symbol> &externs,
                                                 void *out_buffer) {
    auto int_out_buffer = static_cast<int32_t *>(out_buffer);
    Tokenizer tokenizer;
    TranslatorIntoRPN translator;
    ARM32InstructionsEncoder encoder;
    auto rpn_tokens = translator(tokenizer(expression), externs);

    std::unordered_map<int32_t, int> address_position;
    int cur_pos = 1;
    for (const auto &token : rpn_tokens) {
        auto type = token->GetTypeOfToken();
        if (type == TokenType::VARIABLE || type == TokenType::FUNCTION) {
            auto address =
                    std::dynamic_pointer_cast<AddressedRPNToken>(token)
                            ->GetAddress();
            int32_t address_value = reinterpret_cast<intptr_t>(address);
            if (address_position.find(address_value) !=
                address_position.end()) {
                continue;
            }
            int_out_buffer[cur_pos] = address_value;
            address_position.insert(std::make_pair(address_value, cur_pos));
            ++cur_pos;
        } else if (type == TokenType::LITERAL) {
            auto value = std::dynamic_pointer_cast<LiteralRPNToken>(token)
                    ->GetValue();
            if (address_position.find(value) != address_position.end()) {
                continue;
            }
            int_out_buffer[cur_pos] = value;
            address_position.insert(std::make_pair(value, cur_pos));
            ++cur_pos;
        }
    }
    int_out_buffer[0] = encoder.EncodeB(cur_pos);
    int_out_buffer[cur_pos++] = encoder.EncodePushOnStack(4);
    int_out_buffer[cur_pos++] = encoder.EncodePushOnStack(14);

    for (const auto &token : rpn_tokens) {
        auto type = token->GetTypeOfToken();
        if (type == TokenType::VARIABLE) {
            auto address =
                    std::dynamic_pointer_cast<VariableRPNToken>(token)
                            ->GetAddress();
            int32_t address_value = reinterpret_cast<intptr_t>(address);
            auto var_position = address_position[address_value];
            int_out_buffer[cur_pos] =
                    encoder.EncodeLDRFromPC(0, cur_pos, var_position);
            ++cur_pos;
            int_out_buffer[cur_pos] = encoder.EncodeLDRWithoutOffset(0, 0);
            ++cur_pos;
        } else if (type == TokenType::LITERAL) {
            auto value = std::dynamic_pointer_cast<LiteralRPNToken>(token)
                    ->GetValue();
            auto var_position = address_position[value];
            int_out_buffer[cur_pos] =
                    encoder.EncodeLDRFromPC(0, cur_pos, var_position);
            ++cur_pos;
        } else if (type == TokenType::FUNCTION) {
            auto address =
                    std::dynamic_pointer_cast<FunctionRPNToken>(token)
                            ->GetAddress();
            int32_t address_value = reinterpret_cast<intptr_t>(address);
            auto func_position = address_position[address_value];
            int_out_buffer[cur_pos] =
                    encoder.EncodeLDRFromPC(4, cur_pos, func_position);
            ++cur_pos;
            auto cnt_args =
                    std::dynamic_pointer_cast<FunctionRPNToken>(token)
                            ->GetArgsCount();
            for (; cnt_args > 0; --cnt_args) {
                int_out_buffer[cur_pos++] =
                        encoder.EncodePopFromStack(cnt_args - 1);
            }
            int_out_buffer[cur_pos++] = encoder.EncodeMov(14, 15);
            int_out_buffer[cur_pos++] = encoder.EncodeBX(4);
        } else {
            int_out_buffer[cur_pos++] = encoder.EncodePopFromStack(1);
            int_out_buffer[cur_pos++] = encoder.EncodePopFromStack(0);
            switch (type) {
                case OP_PLUS:
                    int_out_buffer[cur_pos++] = encoder.EncodeAdd(0, 0, 1);
                    break;
                case OP_MINUS:
                    int_out_buffer[cur_pos++] = encoder.EncodeSub(0, 0, 1);
                    break;
                case OP_MULT:
                    int_out_buffer[cur_pos++] = encoder.EncodeMul(0, 0, 1);
                    break;
            }
        }
        int_out_buffer[cur_pos++] = encoder.EncodePushOnStack(0);
    }

    int_out_buffer[cur_pos++] = encoder.EncodePopFromStack(0);

    int_out_buffer[cur_pos++] = encoder.EncodePopFromStack(14);
    int_out_buffer[cur_pos++] = encoder.EncodePopFromStack(4);
    int_out_buffer[cur_pos++] = encoder.EncodeBX(14);
}

void jit_compile_expression_to_arm(const char *expression, const Symbol *externs, void *out_buffer) {
    JITCompilerIntoARM32 compiler;
    std::vector<Symbol> externs_vector;
    for (int i = 0; externs[i].pointer != nullptr; ++i) {
        externs_vector.push_back(externs[i]);
    }
    compiler.CompileIntoAllocatedMemory(expression, externs_vector, out_buffer);
}
