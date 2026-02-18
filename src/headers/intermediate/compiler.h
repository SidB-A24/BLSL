//
// Created by Sid B on 06/01/26.
//

#ifndef BLSLANG_COMPILER_H
#define BLSLANG_COMPILER_H

#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "astnodes.h"
#include "../core/blsbyc.h"

namespace BLSL
{
    namespace Precursor
    {

        enum class OperandType
        {
            VIRTUAL_REGISTER_GENERAL,
            VIRTUAL_REGISTER_SCI,

            REGISTER_GENERAL,
            REGISTER_SCI,

            COMPILE_TIME_SIZE,
            LITERAL,
            VARIABLE,
            STACK_INDEX,
        };
        struct Operand
        {
            OperandType type;
            size_t index;
        };
        struct Instruction
        {
            BLSVM::Bytecode::OpCode opCode;
            Operand a;
            Operand b;
            Operand c;
            BLSVM::Bytecode::flag_t flags;
        };

        using PrecursorBuffer_t = std::unique_ptr<std::vector<Instruction>>;

    }

    class Flattener : public ASTNode::Visitor
    {
    private:
        size_t _virtualRegisterIndex;
        std::unordered_map<size_t, size_t> _registerLifetimes;                             // Register Index, Instruction index of last use.

        Precursor::PrecursorBuffer_t _precursorBuffer;

        std::unordered_map<std::string, std::pair<size_t, size_t>> _variableMap;           // Identifier, {size, index}
        size_t _variableIndex;

        std::unordered_map<std::string, size_t> _literalMap;             // Literal, index
        size_t _literalIndex;

        std::unordered_map<size_t, size_t> _compileTimeSizes;            // csz, index
        size_t _compileTimeSizeIndex;

    private:
        Precursor::Operand _traverse_expression(ASTNode::Node* node);
        size_t _cling_variable(ASTNode::Variable* node);

    public:
        Flattener();

        Precursor::PrecursorBuffer_t get_precursor_buffer() {return std::move(_precursorBuffer);}
        std::unordered_map<std::string, size_t> get_literal_map() {return _literalMap;}                             // Yes these are expensive but they're not recurring calls so its okay.
        std::unordered_map<size_t, size_t> get_compile_time_size_map() {return _compileTimeSizes;}                  // Same as above

    public:
        void visit(ASTNode::Alloc *node) override;
        void visit(ASTNode::BinaryOperator *node) override;

    };

    class RegisterPass
    {
    private:
        std::unordered_map<size_t, size_t> _virtualRegisterLifetimes;                  // vRegister Index, Instruction index of last use.
        Precursor::PrecursorBuffer_t _precursorBuffer;

        std::queue<size_t> _freeGeneralRegisters;
        std::unordered_map<size_t, size_t> _assignedRegisters;                             // vreg index, real register index.

    private:
        bool _free_register(Precursor::Operand op, size_t instructionIndex);
        auto _assign_register(Precursor::Operand op, size_t instructionIndex) -> void;
        void _mutate_precursor(Precursor::Operand& op, size_t instructionIndex);
    public:
        RegisterPass(Precursor::PrecursorBuffer_t precursorBuffer, std::unordered_map<size_t, size_t> registerLifetimes);

        void assign_real_registers();


    };

    class Encoder
    {
    private:
        Precursor::PrecursorBuffer_t _precursorBuffer;
        std::unordered_map<size_t, size_t> _compileTimeSizes;
        std::unordered_map<std::string, size_t> _literalMap;

        std::ostream& _outStream;

    public:
        Encoder(Precursor::PrecursorBuffer_t precursorBuffer, std::unordered_map<std::string, size_t> literalMap, std::unordered_map<size_t, size_t> compileTimeSizes, std::ostream& outStream);

        std::ostream& write_out();
    };
}


#endif //BLSLANG_COMPILER_H