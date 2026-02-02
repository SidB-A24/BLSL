//
// Created by Sid B on 06/01/26.
//

/*  Note, integers are stored from LSB to MSB left to right, to simplify operations on them.
 *
 */

#ifndef BLSLANG_BLSBYC_H
#define BLSLANG_BLSBYC_H


#include <cstdint>
#include <cstddef>


namespace BLSVM
{

    using ubyte_t = uint8_t;
    using ulongbyte_t = uint16_t;

    inline constexpr ulongbyte_t UNSIGNED_BYTE_MASK = 0x00FF;

    using sbyte_t = uint8_t;
    using slongbyte_t = int16_t;

    namespace Bytecode
    {
        using section_t = uint8_t;

        enum class Section: uint8_t
        {
            COMPILE_TIME_SIZES,
            LITERALS,
            INSTRUCTIONS,
        };


        using instruction_t = uint64_t;
        using opcode_t = uint8_t;
        using operand_t = uint16_t;
        using flag_t = uint8_t;

        struct Instruction
        {
            opcode_t opcode = 0;
            operand_t a = 0;
            operand_t b = 0;
            operand_t c = 0;
            flag_t flags = 0;
        };

        inline constexpr instruction_t OPCODE_MASK = 0xFF00000000000000;
        inline constexpr instruction_t OPND_A_MASK = 0x00FFFF0000000000;
        inline constexpr instruction_t OPND_B_MASK = 0x000000FFFF000000;
        inline constexpr instruction_t OPND_C_MASK = 0x0000000000FFFF00;
        inline constexpr instruction_t FLAGBT_MASK = 0x00000000000000FF;

        inline constexpr operand_t OPND_TYPE_MASK = 0b1000'0000'0000'0000;                      //Tells the VM whether this points to a register (1) or constant pool (0).

        inline constexpr uint8_t OPCODE_SHIFT = 56;
        inline constexpr uint8_t OPND_A_SHIFT = 40;
        inline constexpr uint8_t OPND_B_SHIFT = 24;
        inline constexpr uint8_t OPND_C_SHIFT = 8;
        inline constexpr uint8_t FLAGBT_SHIFT = 0;

        inline constexpr uint8_t OPND_T_SHIFT = 15;

        enum class OpCode : opcode_t
        {
            EMPTY = 0,

            UNSIGNED_ADD,
            UNSIGNED_SUB,
            UNSIGNED_MUL,
            UNSIGNED_DIV,

            ADD,
            SUB,
            MUL,
            DIV,

            ADD_SCI,
            SUB_SCI,
            MUL_SCI,
            DIV_SCI,

            ALLOC_STACK,
            ALLOC_HEAP,

            CLING_STACK,              // This will tell a register to cling to a value from the stack/heap
            CLING_HEAP,
            JUMP,

            ARG_DENOTE,         // This will denote a register as an argument to be used in the current call frame.

            DEBUG_DUMP,         // Dumps value to the console.
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

        instruction_t make_instruction(                                      // Makes Instruction
            opcode_t opcode = static_cast<opcode_t>(OpCode::EMPTY),
            operand_t a=0,
            operand_t b=0,
            operand_t c=0,
            flag_t flags=0
            );
        bool is_register(operand_t operand);                                 // Checks if the operand is a register or a constant pool reference
        inline void set_register_flag(operand_t& operand, bool value);              // Sets the register flag in the operand.

        opcode_t decode_opcode(const instruction_t& instruction);
        operand_t decode_operand_A(const instruction_t& instruction);
        operand_t decode_operand_B(const instruction_t& instruction);
        operand_t decode_operand_C(const instruction_t& instruction);
        flag_t decode_flag(const instruction_t& instruction);

        Instruction decode_instruction(const instruction_t& instruction);


    }


    namespace Bytecode
    {
        struct ConstEntry
        {
            size_t size;                                                            // Printed out as sizeof(size_t) characters followed by size characters.
            ubyte_t* data;
        };
    }

}
#endif //BLSLANG_BLSBYC_H
