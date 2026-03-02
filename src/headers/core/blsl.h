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
        UNSIGNED_ADD,
        UNSIGNED_SUB,
        UNSIGNED_DIV,
        UNSIGNED_MUL,

        SIGNED_ADD,
        SIGNED_SUB,
        SIGNED_DIV,
        SIGNED_MUL,

        SCI_ADD,
        SCI_SUB,
        SCI_DIV,
        SCI_MUL,

        POW,

        BW_AND,
        BW_OR,
        BW_LSHIFT,
        BW_RSHIFT,
        BW_XOR,
        BW_NOT
    };

    enum class PunctuatorType
    {
        COMMA,
        SEMICOLON,
        COLON,
        LPAREN,
        RPAREN,
        LSQUARE,
        RSQUARE,
        LBRACE,
        RBRACE,
    };


    enum class KeywordType
    {
        IDENTIFIER_PLACEHOLDER,

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
        BIN,
        HEX,
        SCI,
        ASCII
    };

    template<typename T>
    concept BLSLEnum =
        std::same_as<T, TokenType> ||
    std::same_as<T, OperatorType>   ||
    std::same_as<T, PunctuatorType> ||
    std::same_as<T, KeywordType> ||
    std::same_as<T, ComparatorType> ||
    std::same_as<T, LiteralType>;



}

namespace BLSL
{
    using TokenSubType_t = std::variant<OperatorType, ComparatorType, KeywordType, LiteralType, PunctuatorType> ;
    using LexemeTable_t = std::unordered_map<TokenType, std::vector<std::pair <std::string, TokenSubType_t>>>;


    extern const LexemeTable_t LEXEME_TABLE;

    // int priority. * has higher priority than +. These ints are shared across the precedence table
    // i.e. comparators precedence can be checked with operator precedence
    extern const std::unordered_map<OperatorType, std::pair<int, int>> OPERATOR_PRECEDENCE;
    extern const std::unordered_map<OperatorType, int> PREFIX_PRECEDENCE;
    extern const std::unordered_map<ComparatorType, std::pair<int, int>> COMPARATOR_PRECEDENCE;
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

        return std::formatter<std::string>::format(ss.str(), ctx);
    }

};
namespace std
{
    inline ostream& operator<<(ostream& os, const BLSL::Token tok)
    {
        int subTypeCode = std::visit(
            [](auto x)
            {
                return static_cast<int>(x);
            },
            tok.subType
        );

        os << "<Token: " << static_cast<int>(tok.type) << "," << subTypeCode;
        if (tok.value.has_value())
        {
            os << " '" << tok.value.value() << "'";
        }
        os << "> [";
        os << tok.debugPos.line << ":" << tok.debugPos.column << "]";
        return os;
    }

    inline ostream& operator<<(ostream& os, const BLSL::DebugPosition debugPos)
    {
        os << "Line: " << debugPos.line << " Column: " << debugPos.column;
        return os;
    }
}


#endif //BLSLANG_COREDEF_H