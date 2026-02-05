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
        return Precursor::Operand{Precursor::OperandType::VIRTUAL_REGISTER, _virtualRegisterIndex-1};
    }
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
    instruction.c = Precursor::Operand{Precursor::OperandType::VIRTUAL_REGISTER, _virtualRegisterIndex};
    _registerLifetimes.emplace(_virtualRegisterIndex++, _precursorBuffer->size()-1);

    _precursorBuffer->emplace_back(instruction);
}

BLSL::RegisterPass::RegisterPass(Precursor::PrecursorBuffer_t precursorBuffer, std::unordered_map<size_t, size_t> registerLifetimes)
: _precursorBuffer(std::move(precursorBuffer)),
_virtualRegisterLifetimes(std::move(registerLifetimes))
{}

void BLSL::RegisterPass::assign_real_registers()
{

}

