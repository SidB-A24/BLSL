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