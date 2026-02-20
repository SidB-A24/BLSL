//
// Created by Sid B on 06/01/26.
//

#include "../../headers/intermediate/lexer.h"

#include <print>
#include <ranges>
#include <array>
#include <utility>

static const std::string LINE_COMMENT_START = "//";
static const std::string BLOCK_COMMENT_START = "/*";
static const std::string BLOCK_COMMENT_END = "*/";

static const std::unordered_map<char, char> ESCAPE_SEQUENCES = {
    {'n', '\n'},
    {'t', '\t'},
    {'r', '\r'},
    {'\\', '\\'},
    {'"', '"'}
};


void BLSL::Lexer::_step(const size_t n)
{
    _carat += n;
    _debugPos.column += n;
}

void BLSL::Lexer::_newline()
{
    _debugPos.line++;
    _debugPos.column = 0;
}

std::optional<BLSL::Token> BLSL::Lexer::_lex_empty()
{
    while (true)
    {
        std::string ch = _source.substr(_carat, 2);

        if (ch[0] == ' ')
        {
            _step();
            continue;
        }

        if (ch == "\r\n")
        {
            _step(2);
            _newline();
            continue;
        }
        if (ch[0] == '\n' || ch[0] == '\r')
        {
            _step();
            _newline();
            continue;
        }
        if (ch == LINE_COMMENT_START)
        {
            while (ch[0] != '\r' && ch[0] != '\n')
            {
                _step();
                ch = _source.substr(_carat, 1);
            }
            continue;
        }

        if (ch == BLOCK_COMMENT_START)
        {
            while (ch != BLOCK_COMMENT_END)
            {
                _step();
                ch = _source.substr(_carat, 2);
            }
            continue;
        }


        return std::nullopt;
    }
}

std::optional<BLSL::Token> BLSL::Lexer::_lex_long_operator()
{
    std::string ch = _source.substr(_carat, 2);

    auto view = std::views::join(std::array{
        LEXEME_TABLE.at(TokenType::OPERATOR),
        LEXEME_TABLE.at(TokenType::COMPARATOR),
        LEXEME_TABLE.at(TokenType::PUNCTUATOR)});

    for (auto pair : view)
    {
        std::string opLexeme = pair.first;
        if (ch == opLexeme)
        {
            Token token;

            token.type =  std::holds_alternative<OperatorType>(pair.second) ? TokenType::OPERATOR  :
            std::holds_alternative<ComparatorType>(pair.second) ? TokenType::COMPARATOR : TokenType::PUNCTUATOR;

            token.subType = pair.second;
#ifndef NDEBUG
            token.value = ch;
#endif
            token.debugPos = _debugPos;
            _step(2);
            return token;
        }
    }

    return std::nullopt;
}

std::optional<BLSL::Token> BLSL::Lexer::_lex_string()
{
    std::string ch = _source.substr(_carat, 2);

    Token token;

    if (ch[0] != '"') return std::nullopt;

    token.type = TokenType::LITERAL;
    token.subType = LiteralType::ASCII;
    token.debugPos = _debugPos;
    token.value = "";

    _step();
    ch = _source.substr(_carat, 2);

    while (ch[0] != '"')
    {
        ch = _source.substr(_carat, 2);
        if (ch[0] == '\\')
        {
            if (ESCAPE_SEQUENCES.count(ch[1]))
            {
                token.value->push_back(ESCAPE_SEQUENCES.at(ch[1]));
                _step(2);
                continue;
            }
            *token.value += ch;
            _step(2);

        }

        token.value->push_back(ch[0]);
        _step();
    }

    return token;
}


std::optional<BLSL::Token> BLSL::Lexer::_lex_number()
{
    std::string ch = _source.substr(_carat, 2);
    if (!std::isdigit(ch[0])) return std::nullopt;

    Token token;
    token.debugPos = _debugPos;
    token.type = TokenType::LITERAL;
    token.value = "";

    if (ch == "0b")
    {
        token.subType = LiteralType::BIN;
        _step(2);
        ch = _source.substr(_carat, 2);

        while (ch[0] == '0' || ch[0] == '1')
        {
            token.value->push_back(ch[0]);
            _step();
            ch = _source.substr(_carat, 1);
        }

        //padding. For example 101 becomes 00000101. 11101110 remains 11101110
        token.value->insert(token.value->begin(), token.value->size() % 8, '0');

        return token;
    }

    if (ch == "0x")
    {
        token.subType = LiteralType::HEX;
        _step(2);
        ch = _source.substr(_carat, 2);

        while (ishexnumber(ch[0]))
        {
            token.value->push_back(ch[0]);
            _step();
            ch = _source.substr(_carat, 1);
        }

        //padding. For example F becomes 0F. AF remains AF.
        token.value->insert(token.value->begin(),token.value->size() % 2,  '0');

        return token;
    }

    token.subType = LiteralType::INT;
    while (std::isdigit(ch[0]))
    {
        token.value->push_back(ch[0]);
        _step();
        ch = _source.substr(_carat, 1);
    }

    return token;
}


std::optional<BLSL::Token> BLSL::Lexer::_lex_keyword_identifier()
{
    std::string ch = _source.substr(_carat, 1);

    if (!(std::isalnum(ch[0]) || ch[0] == '_')) return std::nullopt;

    Token token;
    token.debugPos = _debugPos;
    token.value = "";

    while (isalnum(ch[0]) || ch[0] == '_')
    {
        token.value->push_back(ch[0]);
        _step();
        ch = _source.substr(_carat, 1);
    }

    for (auto pair : LEXEME_TABLE.at(TokenType::KEYWORD))
    {
        std::string keyword = pair.first;
        if (keyword == token.value)
        {
            token.type = TokenType::KEYWORD;
            token.subType = pair.second;
            return token;
        }
    }

    token.type = TokenType::IDENTIFIER;
    token.subType = KeywordType::IDENTIFIER_PLACEHOLDER;
    return token;
}

std::optional<BLSL::Token> BLSL::Lexer::_lex_single_operator()
{
    std::string ch = _source.substr(_carat, 1);

    auto view = std::views::join(std::array{
        LEXEME_TABLE.at(TokenType::OPERATOR),
        LEXEME_TABLE.at(TokenType::COMPARATOR),
        LEXEME_TABLE.at(TokenType::PUNCTUATOR)
    });

    for (auto pair : view)
    {
        std::string op = pair.first;
        if (op == ch)
        {
            Token token;
            token.debugPos = _debugPos;
            token.value = "";

            token.type =  std::holds_alternative<OperatorType>(pair.second) ? TokenType::OPERATOR  :
            std::holds_alternative<ComparatorType>(pair.second) ? TokenType::COMPARATOR : TokenType::PUNCTUATOR;

            token.subType = pair.second;
            _step();
            return token;
        }
    }

    return std::nullopt;
}

BLSL::Lexer::Lexer()
    :_carat(0), _debugPos({}),
    _SUB_LEXERS({
    &Lexer::_lex_empty,
    &Lexer::_lex_long_operator,
    &Lexer::_lex_string,
    &Lexer::_lex_number,
    &Lexer::_lex_keyword_identifier,
    &Lexer::_lex_single_operator
    })
{

}

void BLSL::Lexer::mount_source_from_file(const std::string &path)
{
    std::ifstream scriptFile(path, std::ios::in | std::ios::binary);

    if (!scriptFile)
    {
        throw Error::FileNotFound(path);
    }

    scriptFile.seekg(0, std::ios::end);
    size_t length = scriptFile.tellg();
    scriptFile.seekg(0, std::ios::beg);

    _source.resize(length + 100);
    scriptFile.read(_source.data(), static_cast<std::streamsize>(length));
    scriptFile.close();

    _carat = 0;
    _debugPos = {1, 0};
}

void BLSL::Lexer::mount_source_from_string(std::string &&source)
{
    _source = std::move(source);
}

std::unique_ptr<std::vector<BLSL::Token>> BLSL::Lexer::lex()
{
    if (_source.empty()) return nullptr;
    std::vector<BLSL::Token> out;

    while (_carat < _source.size() - 100)
    {
        size_t oldPos = _carat;

        for (auto subLexer : _SUB_LEXERS)
        {

            auto result = (this->*subLexer)();

            if (result.has_value())
            {
                out.push_back(result.value());
                break;
            }
        }

        if (_carat != oldPos) continue;

        throw Error::UnidentifiedSequence(_debugPos);
    }


    return std::make_unique<std::vector<BLSL::Token>>(out);
}

void BLSL::Lexer::lex_to_file(const std::string &path)
{
    if (_source.empty()) return;

    std::ofstream outFile(path, std::ios::out | std::ios::binary);

    while (_carat < _source.size() - 100)
    {
        size_t oldPos = _carat;

        for (auto subLexer : _SUB_LEXERS)
        {

            auto result = (this->*subLexer)();

            if (result.has_value())
            {
                outFile << std::format("{}\n", result.value());
                break;
            }
        }

        if (_carat != oldPos) continue;

        outFile.close();
        throw Error::UnidentifiedSequence(_debugPos);
    }

    outFile.close();
}




