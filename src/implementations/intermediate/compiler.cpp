//
// Created by Sid B on 06/01/26.
//

#include "../../headers/intermediate/compiler.h"

#include "../../headers/runtime/vm.h"

static const std::unordered_map<BLSL::OperatorType, BLSVM::Bytecode::OpCode> OPERATOR_OPCODE_MAP = {
    {BLSL::OperatorType::UNSIGNED_ADD, BLSVM::Bytecode::OpCode::UNSIGNED_ADD}
}; //TODO COMPLETE

BLSL::Precursor::Operand BLSL::Flattener::_traverse_expression(ASTNode::Node* node)
{
    if (auto literal = dynamic_cast<ASTNode::Literal*>(node))
    {
        if (!_literalMap.contains(literal->value)) _literalMap.emplace(literal->value, _literalIndex++);
        return Precursor::Operand{Precursor::OperandType::LITERAL, _literalMap.at(literal->value)};
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

size_t BLSL::Flattener::_cling_variable(ASTNode::Variable *node)
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

void BLSL::RegisterPass::_mutate_precursor(Precursor::Operand &op, size_t instructionIndex)
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
: _precursorBuffer(std::move(precursorBuffer)),
_virtualRegisterLifetimes(std::move(registerLifetimes))
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

BLSL::Encoder::Encoder(Precursor::PrecursorBuffer_t precursorBuffer, std::unordered_map<std::string, size_t> literalMap,
    std::unordered_map<size_t, size_t> compileTimeSizes, std::ostream &outStream)
: _precursorBuffer(std::move(precursorBuffer)), _compileTimeSizes(std::move(compileTimeSizes)), _literalMap(std::move(literalMap)), _outStream(outStream)
{

}

