#include "arm32_instructions_encoder.h"

int ARM32InstructionsEncoder::EncodeB(unsigned int bytes_offset) {
    int instruction = static_cast<int>(0xEA000000);
    instruction += bytes_offset - 2;
    return instruction;
}

int ARM32InstructionsEncoder::EncodeLDRFromPC(int dest_register, int cur_row, int var_position) {
    int instruction = static_cast<int>(0xE51F0000);
    int offset = 8 + (cur_row - var_position) * 4;
    instruction += (dest_register << 12) + offset;
    return instruction;
}

int ARM32InstructionsEncoder::EncodeLDRWithoutOffset(int base_register, int dest_register) {
    int instruction = static_cast<int>(0xE5900000);
    instruction += (base_register << 16) + (dest_register << 12);
    return instruction;
}

int ARM32InstructionsEncoder::EncodePushOnStack(int register_num) {
    int instruction = static_cast<int>(0xE52D0000);
    int offset = 4;
    instruction += (register_num << 12) + offset;
    return instruction;
}

int ARM32InstructionsEncoder::EncodePopFromStack(int register_num) {
    int instruction = static_cast<int>(0xE49D0000);
    int offset = 4;
    instruction += (register_num << 12) + offset;
    return instruction;
}

int ARM32InstructionsEncoder::EncodeAdd(int dest_register, int first_register, int second_register) {
    int instruction = static_cast<int>(0xE0800000);
    instruction +=
            (first_register << 16) + (dest_register << 12) + second_register;
    return instruction;
}

int ARM32InstructionsEncoder::EncodeSub(int dest_register, int first_register, int second_register) {
    int instruction = static_cast<int>(0xE0400000);
    instruction +=
            (first_register << 16) + (dest_register << 12) + second_register;
    return instruction;
}

int ARM32InstructionsEncoder::EncodeMul(int dest_register, int first_register, int second_register) {
    int instruction = static_cast<int>(0xE0000090);
    instruction +=
            (dest_register << 16) + (first_register << 8) + second_register;
    return instruction;
}

int ARM32InstructionsEncoder::EncodeBX(int register_num) {
    int instruction = static_cast<int>(0xE12FFF10);
    instruction += register_num;
    return instruction;
}

int ARM32InstructionsEncoder::EncodeMov(int dest_register, int src_register) {
    int instruction = static_cast<int>(0xE1A00000);
    instruction += (dest_register << 12) + src_register;
    return instruction;
}
