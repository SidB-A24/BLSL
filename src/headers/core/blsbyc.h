//
// Created by Sid B on 06/01/26.
//

#include <cstdint>


#ifndef BLSLANG_BLSBYC_H
#define BLSLANG_BLSBYC_H
namespace BLSL
{
    namespace Bytecode
    {
        using instruction_t = uint64_t;
        using opcode_t = uint8_t;
        using operand_t = uint16_t;
        using flag_t = uint8_t;

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

            CLING,              // This will tell a register to cling to a value from the stack/heap
            JUMP,

            ARG_DENOTE,

        };

        enum class Flag : operand_t
        {

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
}
#endif //BLSLANG_BLSBYC_H