//
// Created by Sid B on 06/01/26.
//

#include "../../headers/core/blsbyc.h"
#include "../../headers/core/blsl.h"

namespace BLSL
{
    const LexemeTable_t LEXEME_TABLE = {
        {TokenType::OPERATOR, {
            {"+", OperatorType::ADD},
            {"-", OperatorType::SUB},
            {"*", OperatorType::MUL},
            {"/", OperatorType::DIV},
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
        {OperatorType::MUL, {210, 212}},
        {OperatorType::DIV, {210, 212}},
        {OperatorType::ADD, {200, 202}},
        {OperatorType::SUB, {200, 202}},

        {OperatorType::BW_LSHIFT, {130, 132}},
        {OperatorType::BW_RSHIFT, {130, 132}},
        {OperatorType::BW_NOT, {120, 122}},
        {OperatorType::BW_AND, {110, 112}},
        {OperatorType::BW_OR, {100, 102}},
    };

    const std::unordered_map<OperatorType, int> PREFIX_PRECEDENCE = {
        {OperatorType::SUB, 2100},
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

            instruction |= (opcode << OPCODE_SHIFT) & OPCODE_MASK;
            instruction |= (a << OPND_A_SHIFT) & OPND_A_MASK;
            instruction |= (b << OPND_B_SHIFT) & OPND_B_MASK;
            instruction |= (c << OPND_C_SHIFT) & OPND_C_MASK;
            instruction |= (flags << FLAGBT_SHIFT) & FLAGBT_MASK;

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
    }
}