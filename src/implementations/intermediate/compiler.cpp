//
// Created by Sid B on 06/01/26.
//

#include "../../headers/intermediate/compiler.h"

#include <ranges>

#include "../../headers/runtime/vm.h"

static const std::unordered_map<BLSL::OperatorType, BLSVM::Bytecode::OpCode> OPERATOR_OPCODE_MAP = {
    {BLSL::OperatorType::UNSIGNED_ADD, BLSVM::Bytecode::OpCode::UNSIGNED_ADD}
}; //TODO COMPLETE

BLSVM::Bytecode::operand_t BLSL::Precursor::to_primitive_operand(Operand operand)
{
    BLSVM::Bytecode::operand_t outOperand = static_cast<BLSVM::Bytecode::operand_t>(operand.index) & (~BLSVM::Bytecode::OPND_TYPE_MASK);

    outOperand |= (operand.type == OperandType::LITERAL) ? 0 : BLSVM::Bytecode::OPND_TYPE_MASK;

    return outOperand;
}

BLSL::Precursor::Operand BLSL::Flattener::_traverse_expression(ASTNode::Node* node)
{
    if (auto literal = dynamic_cast<ASTNode::Literal*>(node))
    {
        if (!_literalMap.contains(literal->value)) _literalMap[literal->value] = std::make_pair(_literalIndex++, LiteralType());
        return Precursor::Operand{Precursor::OperandType::LITERAL, _literalMap.at(literal->value).first};
    }

    if (auto variable = dynamic_cast<ASTNode::Variable*>(node))
    {
        return Precursor::Operand{Precursor::OperandType::VARIABLE,
            _cling_variable(variable)
        };
    }

    if (auto binaryOperation = dynamic_cast<ASTNode::BinaryOperator*>(node))
    {
        visit(binaryOperation);
        _registerLifetimes[_virtualRegisterIndex-1] = _precursorBuffer->size();
        return Precursor::Operand{Precursor::OperandType::VIRTUAL_REGISTER_GENERAL, _virtualRegisterIndex-1};
    }


    return Precursor::Operand{}; //This should never happen;
}

size_t BLSL::Flattener::_cling_variable(const ASTNode::Variable *node)
{
    if (!_variableMap.contains(node->identifier)) throw std::runtime_error("Variable mapping not exists"); //TODO THROW

    Precursor::Instruction instruction = {
        BLSVM::Bytecode::OpCode::CLING_STACK,
        Precursor::Operand{Precursor::OperandType::VARIABLE, _variableMap.at(node->identifier).second},
    };

    _precursorBuffer->emplace_back(instruction);

    _registerLifetimes.emplace(_virtualRegisterIndex, _precursorBuffer->size()-1);
    return _virtualRegisterIndex++;
}

BLSL::Flattener::Flattener()
    : _virtualRegisterIndex(0), _variableIndex(0), _literalIndex(0), _compileTimeSizeIndex(0)
{}

void BLSL::Flattener::visit(ASTNode::Alloc *node)
{
    if (_variableMap.contains(node->identifier)) throw std::runtime_error("Variable mapping already exists"); //TODO THROW

    _variableMap[node->identifier] = {node->size, _variableIndex};

    Precursor::Instruction instruction = {
        BLSVM::Bytecode::OpCode::ALLOC_STACK,
        {Precursor::OperandType::VARIABLE, _variableIndex++}
    };

    _precursorBuffer->emplace_back(instruction);
}

void BLSL::Flattener::visit(ASTNode::BinaryOperator *node)
{
    Precursor::Instruction instruction = {
        OPERATOR_OPCODE_MAP.at(node->type)
    };

    instruction.a = _traverse_expression(node->left.get());
    instruction.b = _traverse_expression(node->right.get());

    //NOTE THIS MUST BE THE LAST VREG ALLOCATION IN THIS FUNCTION
    instruction.c = Precursor::Operand{Precursor::OperandType::VIRTUAL_REGISTER_GENERAL, _virtualRegisterIndex};
    _registerLifetimes.emplace(_virtualRegisterIndex++, _precursorBuffer->size()-1);

    _precursorBuffer->emplace_back(instruction);
}

bool BLSL::RegisterPass::_free_register(Precursor::Operand op, size_t instructionIndex)
{
    if (op.type == Precursor::OperandType::VIRTUAL_REGISTER_GENERAL)
    {
        if (_virtualRegisterLifetimes.at(op.index) == instructionIndex)
        {
            _freeGeneralRegisters.emplace(_assignedRegisters.at(op.index));
        }
    }
    return false;
}

void BLSL::RegisterPass::_assign_register(Precursor::Operand op, size_t instructionIndex)
{
    if (op.type == Precursor::OperandType::VIRTUAL_REGISTER_GENERAL)
    {
        if (!_assignedRegisters.contains(op.index))
        {
            _assignedRegisters.emplace(op.index, _freeGeneralRegisters.front());
            _freeGeneralRegisters.pop();
        }
    }
}

void BLSL::RegisterPass::_mutate_precursor(Precursor::Operand &op, size_t instructionIndex) const
{
    if (op.type == Precursor::OperandType::VIRTUAL_REGISTER_GENERAL)
    {
        if (_assignedRegisters.contains(op.index))
        {
            op.type = Precursor::OperandType::REGISTER_GENERAL;
            op.index = _assignedRegisters.at(op.index);
        }
        else
        {
            throw;
            //TODO THROW
        }
    }
}

BLSL::RegisterPass::RegisterPass(Precursor::PrecursorBuffer_t precursorBuffer, std::unordered_map<size_t, size_t> registerLifetimes)
: _virtualRegisterLifetimes(std::move(registerLifetimes)),
_precursorBuffer(std::move(precursorBuffer))
{
    size_t currIndex = 0;
    for (const auto& info : BLSVM::REGISTER_INFO)
    {
        if (info.type == BLSVM::RegisterType::GENERAL)
        {
            for (size_t i = 0; i < info.count; i++)
            {
                _freeGeneralRegisters.emplace(i+currIndex);
            }
        }

        currIndex += info.count;
    }
}

void BLSL::RegisterPass::assign_real_registers()
{
    for (size_t i = 0; i < _precursorBuffer->size(); ++i)
    {
        Precursor::Instruction& instruction = _precursorBuffer->at(i);

        _assign_register(instruction.a, i);
        _free_register(instruction.a, i);

        _assign_register(instruction.b, i);
        _free_register(instruction.b, i);

        _assign_register(instruction.c, i);
        _free_register(instruction.c, i);

        _mutate_precursor(instruction.a, i);
        _mutate_precursor(instruction.b, i);
        _mutate_precursor(instruction.c, i);
    }

}

void BLSL::Encoder::write_section_header(BLSVM::Bytecode::Section section) const
{
    auto encodedSection = static_cast<BLSVM::Bytecode::section_t>(section);
    _outStream.write(reinterpret_cast<const char*>(&encodedSection), sizeof(BLSVM::Bytecode::section_t));
}

void BLSL::Encoder::write_instruction(BLSVM::Bytecode::Instruction instruction) const
{
    BLSVM::Bytecode::instruction_t outInstruction = BLSVM::Bytecode::make_instruction(
    instruction.opcode, instruction.a, instruction.b, instruction.c, instruction.flags
);

    write_val<BLSVM::Bytecode::instruction_t>(outInstruction);
}

void BLSL::Encoder::write_size_t(size_t value) const
{
    _outStream.write(reinterpret_cast<const char*>(&value), sizeof(size_t));
}

std::vector<BLSVM::ubyte_t> BLSL::Encoder::encode_literal(const std::string &value, LiteralType type)
{
    std::vector<BLSVM::ubyte_t> literal;
    switch (type)
    {
        case LiteralType::INT:
            //TODO INT
            break;

        case LiteralType::BIN:
            literal.reserve(value.length()/8);
            for (size_t i = 0; i < value.length(); i += 8)
            {
                BLSVM::ubyte_t ubyte = 0;
                for (size_t j = 0; j < 8; ++j)
                {
                    ubyte |= static_cast<BLSVM::ubyte_t>(value[i+j] == '1' ? 1 : 0) << (7-j);
                }
                literal.push_back(ubyte);
            }
            break;

        case LiteralType::HEX:
            literal.reserve(value.length()/2);
            for (size_t i = 0; i < value.length(); i += 2)
            {
                BLSVM::ubyte_t ubyte = 0;
                auto subs = value.substr(i, 2);
                ubyte = static_cast<BLSVM::ubyte_t>(strtol(subs.data(), nullptr, 16));
                literal.push_back(ubyte);
            }
            break;

        case LiteralType::SCI:
            //TODO SCI
            break;

        case LiteralType::ASCII:
            literal.reserve(value.length());
            for (const char c : value)
            {
                literal.push_back(static_cast<BLSVM::ubyte_t>(c));
            }
            break;
    }

    return literal;
}

BLSL::Encoder::Encoder(Precursor::PrecursorBuffer_t precursorBuffer, Precursor::LiteralMap_t literalMap,
                       Precursor::CompileTimeSizeMap_t compileTimeSizes, std::ostream &outStream)
: _precursorBuffer(std::move(precursorBuffer)), _compileTimeSizes(std::move(compileTimeSizes)), _literalMap(std::move(literalMap)), _outStream(outStream)
{

}

std::ostream & BLSL::Encoder::write_out() const
{
    // Writing out compile time sizes
    write_section_header(BLSVM::Bytecode::Section::COMPILE_TIME_SIZES);
    write_val<size_t>(_compileTimeSizes.size());

    for (const auto &compileTimeSize: _compileTimeSizes | std::views::keys)
    {
        write_val<size_t>(compileTimeSize);
    }

    // Writing out the literals
    write_section_header(BLSVM::Bytecode::Section::LITERALS);
    write_val<size_t>(_literalMap.size());

    for (const auto &literal: _literalMap)
    {
        std::vector<BLSVM::ubyte_t> encodedLiteral = encode_literal(literal.first, literal.second.second);
        write_val<size_t>(encodedLiteral.size());
        for (const auto byte: encodedLiteral)
        {
            write_val<BLSVM::ubyte_t>(byte);
        }
    }

    write_section_header(BLSVM::Bytecode::Section::INSTRUCTIONS);
    // Writing out the code section
    for (auto & precursorInstruction : *_precursorBuffer)
    {
        BLSVM::Bytecode::Instruction instruction{};
        instruction.opcode = static_cast<BLSVM::Bytecode::opcode_t>(precursorInstruction.opCode);

        instruction.a = Precursor::to_primitive_operand(precursorInstruction.a);
        instruction.b = Precursor::to_primitive_operand(precursorInstruction.b);
        instruction.c = Precursor::to_primitive_operand(precursorInstruction.c);

        instruction.flags = precursorInstruction.flags;

        write_instruction(instruction);
    }

    return _outStream;
}

