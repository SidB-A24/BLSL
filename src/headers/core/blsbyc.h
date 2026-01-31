//
// Created by Sid B on 06/01/26.
//

#include <cstdint>


#ifndef BLSLANG_BLSBYC_H
#define BLSLANG_BLSBYC_H
namespace BLSVM
{

    using byte_t = uint8_t;

    namespace Bytecode
    {
        using instruction_t = uint64_t;
        using opcode_t = uint8_t;
        using operand_t = uint16_t;
        using flag_t = uint8_t;

        struct Instruction
        {
            opcode_t opcode;
            operand_t a,b,c;
            flag_t flags;
        };

        inline constexpr instruction_t OPCODE_MASK = 0xFF00000000000000;
        inline constexpr instruction_t OPND_A_MASK = 0x00FFFF0000000000;
        inline constexpr instruction_t OPND_B_MASK = 0x000000FFFF000000;
        inline constexpr instruction_t OPND_C_MASK = 0x0000000000FFFF00;
        inline constexpr instruction_t FLAGBT_MASK = 0x00000000000000FF;

        inline constexpr operand_t OPND_TYPE_MASK = 0b1000'0000'0000'0000;

        inline constexpr uint8_t OPCODE_SHIFT = 56;
        inline constexpr uint8_t OPND_A_SHIFT = 40;
        inline constexpr uint8_t OPND_B_SHIFT = 24;
        inline constexpr uint8_t OPND_C_SHIFT = 8;
        inline constexpr uint8_t FLAGBT_SHIFT = 0;

        inline constexpr uint8_t OPND_T_SHIFT = 15;

        enum class OpCode : opcode_t
        {
            EMPTY = 0,

            ADD,
            SUB,
            MUL,
            DIV,

            ADD_SCI,
            SUB_SCI,
            MUL_SCI,
            DIV_SCI,

            CLING,              // This will tell a register to cling to a value from the stack/heap
            JUMP,

            ARG_DENOTE,         // This will denote a register as an argument to be used in the current call frame.

        };

        //We will define these when necessary. But we have eight possible flags.
        enum class Flag : flag_t
        {
            FLAG1 = 0b00000001,
            FLAG2 = 0b00000010,
            FLAG3 = 0b00000100,
            FLAG4 = 0b00001000,
            FLAG5 = 0b00010000,
            FLAG6 = 0b00100000,
            FLAG7 = 0b01000000,
            FLAG8 = 0b10000000,
        };

        inline instruction_t make_instruction(                                      // Makes Instruction
            opcode_t opcode = static_cast<opcode_t>(OpCode::EMPTY),
            operand_t a=0,
            operand_t b=0,
            operand_t c=0,
            flag_t flags=0
            );
        inline bool is_register(operand_t operand);                                 // Checks if the operand is a register or a constant pool reference
        inline void set_register_flag(operand_t& operand, bool value);              // Sets the register flag in the operand.

    }


    namespace Bytecode
    {
        struct ConstEntry
        {
            size_t size;                                                            // Printed out as sizeof(size_t) characters followed by size characters.
            byte_t* data;
        };
    }

}
#endif //BLSLANG_BLSBYC_H
