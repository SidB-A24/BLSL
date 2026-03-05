//
// Created by Sid B on 06/01/26.
//

#include "../../headers/intermediate/parser.h"

#include <print>

const BLSL::Token& BLSL::Parser::_peek() const
{
    if (_pos != _tokens->size())
    {
        return _tokens->at(_pos);
    }

    return _tokens->back();
}

BLSL::Token BLSL::Parser::_next()
{
    if (_pos != _tokens->size())
    {
        return _tokens->at(_pos++);
    }

    return _tokens->back();
}

std::expected<BLSL::OperatorType, BLSL::Token> BLSL::Parser::_peek_operator() const
{
    auto token = _peek();
    if (token.type == TokenType::OPERATOR)
    {
        return std::get<OperatorType>(token.subType);
    }

    return std::unexpected(token);
}

std::expected<BLSL::Node_t, BLSL::Token> BLSL::Parser::_get_atom()
{
    if (_peek().type == TokenType::LITERAL)
    {
        Token token = _next();
        ASTNode::Literal literal;
        literal.debugPos = token.debugPos;
        literal.value = token.value.value();
        literal.type = std::get<LiteralType>(token.subType);

        return std::make_unique<ASTNode::Literal>(literal);
    }

    if (_peek().type == TokenType::IDENTIFIER)
    {
        Token token = _next();
        ASTNode::Variable variable;
        variable.debugPos = token.debugPos;
        variable.identifier = token.value.value();

        return std::make_unique<ASTNode::Variable>(variable);
    }

    return std::unexpected(_peek());
}

std::expected<std::string, BLSL::Token> BLSL::Parser::_get_identifier()
{
    if (_peek().type == TokenType::IDENTIFIER)
    {
        return _next().value.value();
    }
    return std::unexpected(_peek());
}

std::expected<BLSL::Token, BLSL::Token> BLSL::Parser::_get_literal()
{
    if (_peek().type == TokenType::LITERAL)
    {
        return _next();
    }
    return std::unexpected(_peek());
}

std::expected<std::unique_ptr<BLSL::ASTNode::BinaryOperator>, BLSL::Token> BLSL::Parser::_get_operator()
{
    if (_peek().type == TokenType::OPERATOR)
    {
        const Token token = _next();
        ASTNode::BinaryOperator op;
        op.debugPos = token.debugPos;
        op.type = std::get<OperatorType>(token.subType);

        return std::make_unique<ASTNode::BinaryOperator>(std::move(op));
    }
    return std::unexpected(_peek());
}

bool BLSL::Parser::_match_punctuator(const PunctuatorType pType) const
{
    if (_peek().type == TokenType::PUNCTUATOR)
    {
        if (std::get<PunctuatorType>(_peek().subType) == pType)
        {
            return true;
        }
        return false;
    }
    return false;
}

bool BLSL::Parser::_match_comparator(const ComparatorType cType) const
{
    if (_peek().type == TokenType::COMPARATOR)
    {
        if (std::get<ComparatorType>(_peek().subType) == cType)
        {
            return true;
        }
        return false;
    }
    return false;
}

bool BLSL::Parser::_match_keyword(const KeywordType kType) const
{
    if (_peek().type == TokenType::KEYWORD)
    {
        if (std::get<KeywordType>(_peek().subType) == kType)
        {
            return true;
        }
        return false;
    }
    return false;
}

void BLSL::Parser::_consume_punctuator(const PunctuatorType pType)
{
    if (_peek().type == TokenType::PUNCTUATOR)
    {
        if (const Token token = _next(); std::get<PunctuatorType>(token.subType) != pType)
        {
            throw Error::BadConsume(token);
        }
    }
    else
    {
        throw Error::UnexpectedToken(TokenType::PUNCTUATOR, _peek());
    }
}

void BLSL::Parser::_consume_operator(const OperatorType oType)
{
    if (_peek().type == TokenType::OPERATOR)
    {
        if (const Token token = _next(); std::get<OperatorType>(token.subType) != oType)
        {
            throw Error::BadConsume(token);
        }
    }
    else
    {
        throw Error::UnexpectedToken(TokenType::OPERATOR, _peek());
    }
}

void BLSL::Parser::_consume_comparator(const ComparatorType cType)
{
    if (_peek().type == TokenType::COMPARATOR)
    {
        if (const Token token = _next(); std::get<ComparatorType>(token.subType) != cType)
        {
            throw Error::BadConsume(token);
        }
    }
    else
    {
        throw Error::UnexpectedToken(TokenType::COMPARATOR, _peek());
    }
}

void BLSL::Parser::_consume_keyword(const KeywordType kType)
{
    if (_peek().type == TokenType::KEYWORD)
    {
        if (const Token token = _next(); std::get<KeywordType>(token.subType) != kType)
        {
            throw Error::BadConsume(token);
        }
    }
    else
    {
        throw Error::UnexpectedToken(TokenType::KEYWORD, _peek());
    }
}

size_t BLSL::Parser::_consume_compile_time_size()
{
    _consume(ComparatorType::LESSER);
    const auto literal = _get_literal_consume(LiteralType::INT);

    _consume(ComparatorType::GREATER);
    return std::stoi(literal.value.value());
}

std::vector<size_t> BLSL::Parser::_consume_compile_time_size_list()
{
    std::vector<size_t> result;
    _consume(ComparatorType::LESSER);

    while (!_is_match(ComparatorType::GREATER))
    {
        auto literal = _get_literal_consume(LiteralType::INT);

        result.push_back(std::stoi(literal.value.value()));

        if (!_is_match(ComparatorType::GREATER)) _consume(PunctuatorType::COMMA);
    }


    _consume(ComparatorType::GREATER);

    return result;
}

BLSL::Node_t BLSL::Parser::_make_atom_consume()
{
    if (_peek().type == TokenType::LITERAL)
    {
        Token token = _next();
        ASTNode::Literal literal;
        literal.debugPos = token.debugPos;
        literal.value = token.value.value();
        literal.type = std::get<LiteralType>(token.subType);

        return std::make_unique<ASTNode::Literal>(literal);
    }

    if (_peek().type == TokenType::IDENTIFIER)
    {
        Token token = _next();
        ASTNode::Variable variable;
        variable.debugPos = token.debugPos;
        variable.identifier = token.value.value();

        return std::make_unique<ASTNode::Variable>(variable);
    }

    throw Error::UnexpectedToken(TokenType::LITERAL, _peek());
}

std::string BLSL::Parser::_get_identifier_consume()
{
    if (_peek().type == TokenType::IDENTIFIER)
    {
        return _next().value.value();
    }

    throw Error::UnexpectedToken(TokenType::IDENTIFIER, _peek());
}

BLSL::Token BLSL::Parser::_get_literal_consume()
{
    if (_peek().type == TokenType::LITERAL)
    {
        return _next();
    }
    throw Error::UnexpectedToken(TokenType::LITERAL, _peek());
}

BLSL::Token BLSL::Parser::_get_literal_consume(const LiteralType lType)
{
    if (_peek().type == TokenType::LITERAL)
    {
        if (std::get<LiteralType>(_peek().subType) == lType) return _next();
    }
    throw Error::UnexpectedToken(TokenType::LITERAL, _peek());
}

std::unique_ptr<BLSL::ASTNode::BinaryOperator> BLSL::Parser::_make_binary_operator_consume()
{
    if (_peek().type == TokenType::OPERATOR)
    {
        const Token token = _next();
        ASTNode::BinaryOperator op;
        op.debugPos = token.debugPos;
        op.type = std::get<OperatorType>(token.subType);

        return std::make_unique<ASTNode::BinaryOperator>(std::move(op));
    }
    throw Error::UnexpectedToken(TokenType::OPERATOR, _peek());
}


BLSL::Node_t BLSL::Parser::_parse_for()
{
    const Token head = _next();
    ASTNode::For forNode;
    forNode.debugPos = head.debugPos;

    _consume(PunctuatorType::LPAREN);

    if (_is_match(PunctuatorType::SEMICOLON)) forNode.initialize = std::nullopt;
    else forNode.initialize = _parse_expression();

    _consume(PunctuatorType::SEMICOLON);

    if (_is_match(PunctuatorType::SEMICOLON)) forNode.condition = std::nullopt;
    else forNode.condition = _parse_expression();

    _consume(PunctuatorType::SEMICOLON);

    if (_is_match(PunctuatorType::RPAREN)) forNode.update = std::nullopt;
    else forNode.update = _parse_expression();


    _consume(PunctuatorType::RPAREN);

    forNode.body = _parse_statement();

    return std::make_unique<ASTNode::For>(std::move(forNode));

}

BLSL::Node_t BLSL::Parser::_parse_while()
{
    const Token head = _next();
    ASTNode::While whileNode;
    whileNode.debugPos = head.debugPos;

    _consume(PunctuatorType::LPAREN);

    whileNode.condition = _parse_expression();

    _consume(PunctuatorType::RPAREN);

    whileNode.body = _parse_statement();

    return std::make_unique<ASTNode::While>(std::move(whileNode));
}


BLSL::Node_t BLSL::Parser::_parse_if()
{
    const Token head = _next();
    ASTNode::If ifNode;
    ifNode.debugPos = head.debugPos;

    _consume(PunctuatorType::LPAREN);

    ifNode.condition = _parse_expression();

    _consume(PunctuatorType::RPAREN);

    ifNode.body = _parse_statement();

    if (_is_match(KeywordType::ELSE))
    {
        _consume(KeywordType::ELSE);
        ifNode.elseBranch = _parse_statement();

    }
    else
    {
        ifNode.elseBranch = std::nullopt;
    }

    return std::make_unique<ASTNode::If>(std::move(ifNode));
}

BLSL::Node_t BLSL::Parser::_parse_else()
{
    const Token head = _next();
    throw Error::MissingIf(head.debugPos);
}

BLSL::Node_t BLSL::Parser::_parse_func()
{
    Token head = _next();
    ASTNode::Func funcNode;
    funcNode.debugPos = head.debugPos;

    funcNode.identifier = _get_identifier_consume();

    funcNode.returnSize = _consume_compile_time_size();

    _consume(PunctuatorType::LPAREN);

    while (!_is_match(PunctuatorType::RPAREN))
    {
        FormalParameter formalParam;

        formalParam.identifier = _get_identifier_consume();
        funcNode.parameters.push_back(formalParam);

        if (!_is_match(PunctuatorType::RPAREN)) _consume(PunctuatorType::COMMA);
    }

    _consume(PunctuatorType::RPAREN);

    auto compileSizes = _consume_compile_time_size_list();

    for (size_t i = 0; i < funcNode.parameters.size(); i++)
    {
        //TODO: THROW
        if (i == compileSizes.size()) throw;
        funcNode.parameters[i].size = compileSizes[i];
    }

    funcNode.body = _parse_statement();

    return std::make_unique<ASTNode::Func>(std::move(funcNode));
}

BLSL::Node_t BLSL::Parser::_parse_meminit()
{
    const Token head = _next();
    ASTNode::MemInit memInitNode;
    memInitNode.debugPos = head.debugPos;

    memInitNode.size = _consume_compile_time_size();

    return std::make_unique<ASTNode::MemInit>(std::move(memInitNode));
}

BLSL::Node_t BLSL::Parser::_parse_alloc()
{
    const Token head = _next();
    ASTNode::Alloc allocNode;
    allocNode.debugPos = head.debugPos;

    allocNode.identifier = _get_identifier_consume();
    allocNode.size = _consume_compile_time_size();
    _consume(PunctuatorType::SEMICOLON);

    return std::make_unique<ASTNode::Alloc>(std::move(allocNode));
}

BLSL::Node_t BLSL::Parser::_parse_expression(const int lowestPrecedence)
{
    Node_t LHS;

    if (_peek().type == TokenType::PUNCTUATOR)
    {
        switch (std::get<PunctuatorType>(_peek().subType))
        {
            case PunctuatorType::LPAREN:
                _next();

                LHS = _parse_expression();

                _consume(PunctuatorType::RPAREN);
                break;
            case PunctuatorType::SEMICOLON:
            {
                throw Error::UnexpectedToken(TokenType::IDENTIFIER, _peek());
            }
            default:
                throw Error::UnexpectedToken(TokenType::PUNCTUATOR, _peek());
                break;
        }
    }
    else if (_peek().type == TokenType::OPERATOR)
    {
        if (PREFIX_PRECEDENCE.contains(std::get<OperatorType>(_peek().subType)))
        {
            const auto token = _next();
            ASTNode::UnaryOperator unaryOperator;
            unaryOperator.debugPos = token.debugPos;
            unaryOperator.type = std::get<OperatorType>(token.subType);

            unaryOperator.right = _parse_expression(PREFIX_PRECEDENCE.at(std::get<OperatorType>(token.subType)));

            return std::make_unique<ASTNode::UnaryOperator>(std::move(unaryOperator));
        }
    }
    else
    {
        LHS = std::move(_make_atom_consume());
    }



    while (true)
    {
        if (_peek().type == TokenType::PUNCTUATOR)
        {
            if (std::get<PunctuatorType>(_peek().subType) == PunctuatorType::RPAREN)
            {
                break;
            }
        }

        auto opExpected = _peek_if_type<OperatorType>();
        if (!opExpected.has_value()) break;

        auto [leftPrecedence, rightPrecedence] = OPERATOR_PRECEDENCE.at(opExpected.value());
        if (leftPrecedence < lowestPrecedence) break;

        auto op = _make_binary_operator_consume();

        auto RHS = _parse_expression(rightPrecedence);

        op->left = std::move(LHS);
        op->right = std::move(RHS);

        LHS = std::move(op);

    }

    return std::move(LHS);
}

BLSL::BodyNode_t BLSL::Parser::_parse_block()
{
    _consume(PunctuatorType::LBRACE);
    ASTNode::BodyNode bodyNode;

    while (!_is_match(PunctuatorType::RBRACE))
    {
        bodyNode.nodes.emplace_back(_parse_statement());
    }

    _consume(PunctuatorType::RBRACE);

    return std::make_unique<ASTNode::BodyNode>(std::move(bodyNode));
}

BLSL::Node_t BLSL::Parser::_parse_statement()
{
    if (_is_match(PunctuatorType::LBRACE))
    {
        BodyNode_t block = _parse_block();
        return block;
    }
    auto debug_peek = _peek();

    switch (_peek().type)
    {
        //When we have expressions such as a = 3 + 8;
        case TokenType::IDENTIFIER:
        {
            return _parse_expression();
        }
        case TokenType::KEYWORD:
            switch (std::get<KeywordType>(_peek().subType))
            {
            case KeywordType::IDENTIFIER_PLACEHOLDER:
                throw Error::BadToken(_next());

            case KeywordType::FOR:
                return _parse_for();

            case KeywordType::WHILE:
                return _parse_while();

            case KeywordType::IF:
                return _parse_if();

            case KeywordType::ELSE:
                return _parse_else();

            case KeywordType::FUNC:
                return _parse_func();

            case KeywordType::MEMINIT:
                return _parse_meminit();

            case KeywordType::ALLOC:
                return _parse_alloc();
            }


        default:
            throw Error::UnexpectedToken(TokenType::KEYWORD, _next());
    }
}

BLSL::Parser::Parser(std::unique_ptr<std::vector<Token>> in)
    : _tokens(std::move(in)), _pos(0)
{

}

BLSL::Node_t BLSL::Parser::parse()
{
    ASTNode::BodyNode bodyNode;

    while (_pos < _tokens->size())
    {
        //_parse_statement();
        switch (_peek().type)
        {
            //When we have expressions such as a = 3 + 8;
            case TokenType::PUNCTUATOR:
                switch (std::get<PunctuatorType>(_peek().subType))
                {
                    case PunctuatorType::SEMICOLON:
                        _next();
                        continue;
                    default:
                        throw Error::UnexpectedToken(TokenType::KEYWORD, _next());
                }
            default:
                break;
        }

        bodyNode.nodes.emplace_back(_parse_statement());
    }

    return std::make_unique<ASTNode::BodyNode>(std::move(bodyNode));

}
