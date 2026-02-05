//
// Created by Sid B on 06/01/26.
//

#include "../../headers/runtime/vm.h"

#include <fstream>

namespace BLSVM
{

    inline reginfo_t make_reginfo(regtype_t type, regflag_t flags)
    {
        reginfo_t reginfo = 0;

        reginfo |= (static_cast<reginfo_t>(type) << REGISTER_TYPE_SHIFT) & REGISTER_TYPE_MASK;
        reginfo |= (static_cast<reginfo_t>(flags) << REGISTER_FLAG_SHIFT) & REGISTER_FLAG_MASK;

        return reginfo;
    }


    Register& VM::get_register(Bytecode::operand_t operand)
    {
        size_t index = static_cast<size_t>(operand & (~Bytecode::OPND_TYPE_MASK));

        if (index >= REGISTER_COUNT) throw std::runtime_error("Invalid register"); // TODO THROW
        if (!Bytecode::is_register(operand)) throw std::runtime_error("Invalid register"); //TODO THROW

        return _registers[index];
    }

    View VM::view_register(Bytecode::operand_t operand) const
    {
        size_t index = static_cast<size_t>(operand & (~Bytecode::OPND_TYPE_MASK));

        if (index >= REGISTER_COUNT) throw std::runtime_error("Invalid register"); // TODO THROW

        const View value = {
            _registers[index].loc,
            _registers[index].size,
        };

        return value;
    }

    View VM::view_literal(Bytecode::operand_t operand) const
    {
        size_t index = static_cast<size_t>(operand & (~Bytecode::OPND_TYPE_MASK));

        const View literal = {
            read_data(index),
            read_size(index),
        };

        return literal;
    }

    View VM::view_operand(Bytecode::operand_t operand) const
    {
        if (operand & Bytecode::OPND_TYPE_MASK) return view_register(operand);

        else return view_literal(operand);
    }


    VM::VM()
    {
        size_t i = 0;
        for (auto info: REGISTER_INFO)
        {
            for (size_t j = 0; j < info.count && i < REGISTER_COUNT; j++, i++)
            {
                _registers[i].info = make_reginfo(static_cast<regtype_t>(info.type), info.flags);
            }
        }


        _operations[static_cast<size_t>(Bytecode::OpCode::UNSIGNED_ADD)] = &VM::_operation_UNSIGNED_ADD;
        _operations[static_cast<size_t>(Bytecode::OpCode::DEBUG_DUMP)] = &VM::_operation_DEBUG_DUMP;
        _operations[static_cast<size_t>(Bytecode::OpCode::ALLOC_STACK)] = &VM::_operation_ALLOC_STACK;
        _operations[static_cast<size_t>(Bytecode::OpCode::CLING_STACK)] = &VM::_operation_CLING_STACK;

    }

    void VM::defer_load_vm(std::istream &inputStream)
    {
        Bytecode::instruction_t instruction;

        while (inputStream.read(reinterpret_cast<char *>(&instruction), sizeof(instruction)))
        {
            _program.emplace_back(Bytecode::decode_instruction(instruction));
        }
    }

    void VM::boot()
    {
        for (const auto& instruction : _program)
        {
            (this->*_operations[static_cast<size_t>(instruction.opcode)])(instruction);
        }
    }

    void VM::_operation_UNSIGNED_ADD(Bytecode::Instruction instruction)
    {
        if (!Bytecode::is_register(instruction.c)) throw std::runtime_error("Invalid operand for ADD"); //TODO THROW

        auto dest = get_register(instruction.c);

        if (!(dest.info & REG_FLAG_WRITABLE)) throw std::runtime_error("Invalid operand for ADD"); // TODO THROW

        View a = view_operand(instruction.a);
        View b = view_operand(instruction.b);

        ulongbyte_t addResult = 0;

        for (size_t i = 0; i < dest.size; i++)
        {
            ubyte_t aValue = (i < a.size) ? a.loc[i] : 0;
            ubyte_t bValue = (i < b.size) ? b.loc[i] : 0;

            addResult = aValue + bValue + (addResult >> sizeof(ubyte_t)*8);
            dest.loc[i] = static_cast<ubyte_t>(addResult & UNSIGNED_BYTE_MASK);
        }
    }

    void VM::_operation_ALLOC_STACK(Bytecode::Instruction instruction)
    {
        size_t compileTimeSizeIndex = static_cast<size_t>(instruction.a & (~Bytecode::OPND_TYPE_MASK));

        size_t compileTimeSize = CompileTimeSizePool::get_size(compileTimeSizeIndex);

        Stack::push(compileTimeSize);
    }

    void VM::_operation_CLING_STACK(Bytecode::Instruction instruction)
    {
        if (!Bytecode::is_register(instruction.a)) throw std::runtime_error("Invalid operand for CLING"); //TODO THROW
        if (Bytecode::is_register(instruction.b)) throw std::runtime_error("Invalid operand for CLING"); //TODO THROW

        auto& dest = get_register(instruction.a);

        if (!((dest.info & REGISTER_FLAG_MASK) & REG_FLAG_WRITABLE)) throw std::runtime_error("Invalid operand for CLING"); // TODO THROW

        size_t stackIndex = static_cast<size_t>(instruction.b & (~Bytecode::OPND_TYPE_MASK));

        dest.loc = Stack::get_ptr(stackIndex);
        dest.size = Stack::get_size(stackIndex);
        dest.size;
    }

    void VM::_operation_DEBUG_DUMP(Bytecode::Instruction instruction)
    {
        View view = view_operand(instruction.a);
        for (size_t i = 0; i < view.size; i++)
        {
            OutputStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(view.loc[i]) << ' ';
        }
    }
}


