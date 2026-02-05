//
// Created by Sid B on 06/01/26.
//

#include "../../headers/core/blsbyc.h"
#include "../../headers/core/blsl.h"

namespace BLSL
{
    const LexemeTable_t LEXEME_TABLE = {
        {TokenType::OPERATOR, {
            {"+", OperatorType::UNSIGNED_ADD},
            {"-", OperatorType::UNSIGNED_SUB},
            {"*", OperatorType::UNSIGNED_MUL},
            {"/", OperatorType::UNSIGNED_DIV},

            {"~+", OperatorType::SIGNED_ADD},
            {"~-", OperatorType::SIGNED_SUB},
            {"~*", OperatorType::SIGNED_MUL},
            {"~/", OperatorType::SIGNED_DIV},

            {".+", OperatorType::SCI_ADD},
            {".-", OperatorType::SCI_SUB},
            {".*", OperatorType::SCI_MUL},
            {"./", OperatorType::SCI_DIV},

            {"^", OperatorType::POW},
            {"&", OperatorType::BW_AND},
            {"|", OperatorType::BW_OR},
            {"<<", OperatorType::BW_LSHIFT},
            {">>", OperatorType::BW_RSHIFT},
            {"~", OperatorType::BW_NOT},
            {"^", OperatorType::BW_XOR}
        }},
        {TokenType::COMPARATOR, {
            {"&&", ComparatorType::AND},
            {"!", ComparatorType::NOT},
            {"||", ComparatorType::OR},
            {">", ComparatorType::GREATER},
            {"<", ComparatorType::LESSER},
            {"==", ComparatorType::EQUAL}
        }},
        {TokenType::PUNCTUATOR, {
            {",", PunctuatorType::COMMA},
            {";", PunctuatorType::SEMICOLON},
            {":", PunctuatorType::COLON},
            {"(", PunctuatorType::LPAREN},
            {")", PunctuatorType::RPAREN},
            {"[", PunctuatorType::LSQUARE},
            {"]", PunctuatorType::RSQUARE},
            {"{", PunctuatorType::LBRACE},
            {"}", PunctuatorType::RBRACE}
        }},
        {TokenType::KEYWORD, {
            {"meminit", KeywordType::MEMINIT},
            {"alloc", KeywordType::ALLOC},
            {"func", KeywordType::FUNC},
            {"for", KeywordType::FOR},
            {"while", KeywordType::WHILE},
            {"if", KeywordType::IF},
            {"else", KeywordType::ELSE}
        }}
    };

    const std::unordered_map<OperatorType, std::pair<int, int>> OPERATOR_PRECEDENCE = {
        {OperatorType::POW, {220, 222}},
        {OperatorType::UNSIGNED_MUL, {210, 212}},
        {OperatorType::UNSIGNED_DIV, {210, 212}},
        {OperatorType::UNSIGNED_ADD, {200, 202}},
        {OperatorType::UNSIGNED_SUB, {200, 202}},

        {OperatorType::BW_LSHIFT, {130, 132}},
        {OperatorType::BW_RSHIFT, {130, 132}},
        {OperatorType::BW_NOT, {120, 122}},
        {OperatorType::BW_AND, {110, 112}},
        {OperatorType::BW_OR, {100, 102}},
    };

    const std::unordered_map<OperatorType, int> PREFIX_PRECEDENCE = {
        {OperatorType::UNSIGNED_SUB, 2100},
        {OperatorType::BW_NOT, 1100}
    };

    const std::unordered_map<ComparatorType, std::pair<int, int>> COMPARATOR_PRECEDENCE = {
        {ComparatorType::LESSER, {50, 52}},
        {ComparatorType::GREATER, {50, 52}},
        {ComparatorType::EQUAL, {40, 42}},
        {ComparatorType::NOT, {30, 32}},
        {ComparatorType::AND, {20, 22}},
        {ComparatorType::OR, {10, 12}}
    };





}

namespace BLSVM
{
    namespace Bytecode
    {
        instruction_t make_instruction(opcode_t opcode, operand_t a, operand_t b, operand_t c, flag_t flags)
        {
            instruction_t instruction = 0;

            instruction |= static_cast<instruction_t>(opcode) << OPCODE_SHIFT;
            instruction |= static_cast<instruction_t>(a) << OPND_A_SHIFT;
            instruction |= static_cast<instruction_t>(b) << OPND_B_SHIFT;
            instruction |= static_cast<instruction_t>(c) << OPND_C_SHIFT;
            instruction |= static_cast<instruction_t>(flags) << FLAGBT_SHIFT;

            return instruction;
        }

        bool is_register(operand_t operand)
        {
            return operand & OPND_TYPE_MASK;
        }

        void set_register_flag(operand_t& operand, bool value)
        {
            operand |= (value << OPND_T_SHIFT) & OPND_TYPE_MASK;
        }

        opcode_t decode_opcode(const instruction_t &instruction)
        {
            return static_cast<Bytecode::opcode_t>((instruction & Bytecode::OPCODE_MASK) >> Bytecode::OPCODE_SHIFT);
        }

        operand_t decode_operand_A(const instruction_t &instruction)
        {
            return static_cast<Bytecode::operand_t>((instruction & Bytecode::OPND_A_MASK) >> Bytecode::OPND_A_SHIFT);
        }

        operand_t decode_operand_B(const instruction_t &instruction)
        {
            return static_cast<Bytecode::operand_t>((instruction & Bytecode::OPND_B_MASK) >> Bytecode::OPND_B_SHIFT);
        }

        operand_t decode_operand_C(const instruction_t &instruction)
        {
            return static_cast<Bytecode::operand_t>((instruction & Bytecode::OPND_C_MASK) >> Bytecode::OPND_C_SHIFT);
        }

        flag_t decode_flag(const instruction_t &instruction)
        {
            return static_cast<Bytecode::flag_t>((instruction & Bytecode::FLAGBT_MASK) >> Bytecode::FLAGBT_SHIFT);
        }

        Instruction decode_instruction(const instruction_t &instruction)
        {
            Instruction instructionStruct{};
            instructionStruct.opcode = Bytecode::decode_opcode(instruction);
            instructionStruct.a = Bytecode::decode_operand_A(instruction);
            instructionStruct.b = Bytecode::decode_operand_B(instruction);
            instructionStruct.c = Bytecode::decode_operand_C(instruction);
            instructionStruct.flags = Bytecode::decode_flag(instruction);
            return instructionStruct;
        }
    }


    const std::vector<RegisterInfo> REGISTER_INFO = {
        {
            RegisterType::INTERNAL,
            RegisterFlag::REG_FLAG_SPECIAL_WRITEABLE,
            16
        },
        {
            RegisterType::ARGS,
            RegisterFlag::REG_FLAG_PRE_CALL_WRITABLE,
            32
        },
        {
            RegisterType::RETURNS,
            RegisterFlag::REG_FLAG_POST_CALL_WRITEABLE,
            32
        },
        {
            RegisterType::SCI,
            RegisterFlag::REG_FLAG_WRITABLE,
            64
        },
        {
            RegisterType::GENERAL,
            RegisterFlag::REG_FLAG_WRITABLE,
            112
        }
    };

}