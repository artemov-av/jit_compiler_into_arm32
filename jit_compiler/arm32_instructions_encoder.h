#pragma once

class ARM32InstructionsEncoder {
public:
    int EncodeB(unsigned int bytes_offset);

    int EncodeLDRFromPC(int dest_register, int cur_row, int var_position);

    int EncodeLDRWithoutOffset(int base_register, int dest_register);

    int EncodePushOnStack(int register_num);

    int EncodePopFromStack(int register_num);

    int EncodeAdd(int dest_register, int first_register, int second_register);

    int EncodeSub(int dest_register, int first_register, int second_register);

    int EncodeMul(int dest_register, int first_register, int second_register);

    int EncodeBX(int register_num);

    int EncodeMov(int dest_register, int src_register);
};
