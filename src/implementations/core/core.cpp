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

    const std::unordered_map<OperatorType, int> OPERATOR_PRECEDENCE = {
        {OperatorType::POW, 220},
        {OperatorType::MUL, 210},
        {OperatorType::DIV, 210},
        {OperatorType::ADD, 200},
        {OperatorType::SUB, 200},

        {OperatorType::BW_LSHIFT, 130},
        {OperatorType::BW_RSHIFT, 130},
        {OperatorType::BW_NOT, 120},
        {OperatorType::BW_AND, 110},
        {OperatorType::BW_OR, 100},
    };

    const std::unordered_map<ComparatorType, int> COMPARATOR_PRECEDENCE = {
        {ComparatorType::LESSER, 50},
        {ComparatorType::GREATER, 50},
        {ComparatorType::EQUAL, 40},
        {ComparatorType::NOT, 30},
        {ComparatorType::AND, 20},
        {ComparatorType::OR, 10}
    };


}