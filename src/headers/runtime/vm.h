//
// Created by Sid B on 06/01/26.
//


#ifndef BLSLANG_VM_H
#define BLSLANG_VM_H

#include <fstream>

#include "mem.h"
#include <iomanip>

namespace BLSVM
{
    using reginfo_t = uint16_t;

    using regtype_t = uint8_t;
    using regflag_t = uint8_t;

    inline constexpr reginfo_t REGISTER_TYPE_MASK = 0xFF00;
    inline constexpr reginfo_t REGISTER_FLAG_MASK = 0x00FF;

    inline constexpr uint8_t REGISTER_TYPE_SHIFT = 8;
    inline constexpr uint8_t REGISTER_FLAG_SHIFT = 0;

    enum class RegisterType: regtype_t
    {
        INTERNAL,
        ARGS,
        RETURNS,
        SCI,
        GENERAL,
    };

    // Not Enum Class to allow us to use as bitfield.
    enum RegisterFlag: regflag_t
    {
        REG_FLAG_WRITABLE = 0b00000001,
        REG_FLAG_PRE_CALL_WRITABLE = 0b00000010,
        REG_FLAG_POST_CALL_WRITEABLE = 0b00000100,
        REG_FLAG_SPECIAL_WRITEABLE = 0b00001000,
    };

    struct RegisterInfo
    {
        RegisterType type;
        regflag_t flags;
        size_t count;
    };

    inline reginfo_t make_reginfo(regtype_t type, regflag_t flags);

    struct Register
    {
        ubyte_t* loc;
        size_t size;
        reginfo_t info;
    };

    inline constexpr size_t REGISTER_COUNT = 256;
    extern const std::vector<RegisterInfo> REGISTER_INFO;
};



namespace BLSVM
{
    class VM;

    using operation_f = void (VM::*)(const Bytecode::Instruction);

    inline std::ostream& OutputStream = std::cout;

    class VM : public Stack, public LiteralPool, public CompileTimeSizePool
    {
    private:
        std::array<Register, REGISTER_COUNT> _registers{};
        std::array<operation_f, sizeof(Bytecode::opcode_t)*256> _operations{};

        std::vector<Bytecode::Instruction> _program;

    private:
        [[nodiscard]] Register& get_register(Bytecode::operand_t operand);

        [[nodiscard]] View view_register(Bytecode::operand_t operand) const;
        [[nodiscard]] View view_literal(Bytecode::operand_t operand) const;

        [[nodiscard]] View view_operand(Bytecode::operand_t operand) const;


    public:
        VM();
        void defer_load_vm(std::istream& inputStream);

        void boot();

    private:
        void _operation_UNSIGNED_ADD(Bytecode::Instruction instruction);
        void _operation_ALLOC_STACK(Bytecode::Instruction instruction);
        void _operation_CLING_STACK(Bytecode::Instruction instruction);
        void _operation_DEBUG_DUMP(Bytecode::Instruction instruction);
    };
}
#endif //BLSLANG_VM_H