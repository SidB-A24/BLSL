//
// Created by Sid B on 06/01/26.
//

#ifndef BLSLANG_COREDEF_H
#define BLSLANG_COREDEF_H
#include <unordered_map>
#include <variant>
#include <vector>
#include <optional>
#include <sstream>
#include <string>
#include <format>

namespace BLSL
{
    enum class TokenType
    {
        OPERATOR,
        KEYWORD,
        PUNCTUATOR,
        IDENTIFIER,
        LITERAL,
        COMPARATOR
    };

    enum class OperatorType
    {
        ADD,
        SUB,
        POW,
        MUL,
        DIV,

        BW_AND,
        BW_OR,
        BW_LSHIFT,
        BW_RSHIFT,
        BW_XOR,
        BW_NOT
    };


    enum class KeywordType
    {
        MEMINIT,
        ALLOC,
        FUNC,
        FOR,
        IF,
        ELSE,
        WHILE
    };

    enum class ComparatorType
    {
        AND,
        OR,
        NOT,

        GREATER,
        LESSER,
        EQUAL,
    };

    enum class LiteralType
    {
        INT,
        SCI,
        ASCII
    };

}

namespace BLSL
{
    using TokenSubType_t = std::variant<OperatorType, ComparatorType, KeywordType> ;
    using LexemeTable_t = std::unordered_map<TokenType, std::vector<std::pair <std::string, TokenSubType_t>>>;


    extern const LexemeTable_t LEXEME_TABLE;

    // int priority. * has higher priority than +. These ints are shared across the precedence table
    // i.e. comparators precedence can be checked with operator precedence
    extern const std::unordered_map<OperatorType, int> OPERATOR_PRECEDENCE;
    extern const std::unordered_map<ComparatorType, int> COMPARATOR_PRECEDENCE;
}

namespace BLSL
{
    struct DebugPosition
    {
        unsigned line;
        unsigned column;
    };

    struct Token
    {
        TokenType type;
        TokenSubType_t subType;
        std::optional<std::string> value;
        DebugPosition debugPos;
    };

}

// Debug formatter for printing tokens to console.
template<>
struct std::formatter<BLSL::Token> : std::formatter<std::string>
{
    auto format(const BLSL::Token& tok, std::format_context& ctx) const
    {
        int subTypeCode = std::visit(
            [](auto x)
            {
                return static_cast<int>(x);
            },
            tok.subType
        );

        std::stringstream ss;
        ss << "<Token: " << static_cast<int>(tok.type) << "," << subTypeCode;
        if (tok.value.has_value())
        {
            ss << " '" << tok.value.value() << "'";
        }
        ss << "> [";
        ss << tok.debugPos.line << ":" << tok.debugPos.column << "]";
        ss << "\n";

        return std::formatter<std::string>::format(ss.str(), ctx);
    }

};

#endif //BLSLANG_COREDEF_H