//
// Created by Sid B on 06/01/26.
//

#include "../../headers/intermediate/parser.h"

#include <print>

BLSL::Token BLSL::Parser::_peek() const
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
        Token token = _next();
        ASTNode::BinaryOperator op;
        op.debugPos = token.debugPos;
        op.type = std::get<OperatorType>(token.subType);

        return std::make_unique<ASTNode::BinaryOperator>(std::move(op));
    }
}

bool BLSL::Parser::_match_punctuator(PunctuatorType pType) const
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

bool BLSL::Parser::_match_comparator(ComparatorType cType) const
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

bool BLSL::Parser::_match_keyword(KeywordType kType) const
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

void BLSL::Parser::_consume_punctuator(PunctuatorType pType)
{
    if (_peek().type == TokenType::PUNCTUATOR)
    {

        Token token = _next();
        if (std::get<PunctuatorType>(token.subType) != pType)
        {
            //TODO: THrow
            throw;
        }
    }
    else
    {
        //TODO: THrow
        throw;
    }
}

void BLSL::Parser::_consume_operator(OperatorType oType)
{
    if (_peek().type == TokenType::OPERATOR)
    {

        Token token = _next();
        if (std::get<OperatorType>(token.subType) != oType)
        {
            //TODO: THrow
            throw;
        }
    }
    else
    {
        //TODO: THrow
        throw;
    }
}

void BLSL::Parser::_consume_comparator(ComparatorType cType)
{
    if (_peek().type == TokenType::COMPARATOR)
    {
        Token token = _next();
        if (std::get<ComparatorType>(token.subType) != cType)
        {
            //TODO: THrow
            throw;
        }
    }
    else
    {
        //TODO: THrow
        throw;
    }
}

void BLSL::Parser::_consume_keyword(KeywordType kType)
{
    if (_peek().type == TokenType::KEYWORD)
    {
        Token token = _next();
        if (std::get<KeywordType>(token.subType) != kType)
        {
            //TODO: THROW
            throw;
        }
    }
    else
    {
        //TODO: THrow
        throw;
    }
}

size_t BLSL::Parser::_consume_compile_time_size()
{
    _consume_comparator(ComparatorType::LESSER);
    auto literal = _get_literal();
    if (!literal.has_value())
    {
        //TODO: Throw
        throw;
    }

    if (std::get<LiteralType>(literal->subType) != LiteralType::INT)
    {
        //TODO: Throw
        throw;
    }

    _consume_comparator(ComparatorType::GREATER);
    return std::stoi(literal->value.value());
}

std::vector<size_t> BLSL::Parser::_consume_compile_time_size_list()
{
    std::vector<size_t> result;
    _consume_comparator(ComparatorType::LESSER);

    while (!_match_comparator(ComparatorType::GREATER))
    {
        auto literal = _get_literal();
        if (!literal.has_value())
        {
            //TODO: Throw
            throw;
        }

        if (std::get<LiteralType>(literal->subType) != LiteralType::INT)
        {
            //TODO: Throw
            throw;
        }

        result.push_back(std::stoi(literal->value.value()));

        if (!_match_comparator(ComparatorType::GREATER)) _consume_punctuator(PunctuatorType::COMMA);
    }


    _consume_comparator(ComparatorType::GREATER);

    return result;
}

BLSL::Node_t BLSL::Parser::_parse_for()
{
    Token head = _next();
    ASTNode::For forNode;
    forNode.debugPos = head.debugPos;

    _consume_punctuator(PunctuatorType::LPAREN);

    if (_match_punctuator(PunctuatorType::SEMICOLON)) forNode.initialize = std::nullopt;
    else forNode.initialize = _parse_expression();

    _consume_punctuator(PunctuatorType::SEMICOLON);

    if (_match_punctuator(PunctuatorType::SEMICOLON)) forNode.condition = std::nullopt;
    else forNode.condition = _parse_expression();

    _consume_punctuator(PunctuatorType::SEMICOLON);

    if (_match_punctuator(PunctuatorType::RPAREN)) forNode.update = std::nullopt;
    else forNode.update = _parse_expression();


    _consume_punctuator(PunctuatorType::RPAREN);

    forNode.body = _parse_statement();

    return std::make_unique<ASTNode::For>(std::move(forNode));

}

BLSL::Node_t BLSL::Parser::_parse_while() {}


BLSL::Node_t BLSL::Parser::_parse_if()
{
    Token head = _next();
    ASTNode::If ifNode;
    ifNode.debugPos = head.debugPos;

    _consume_punctuator(PunctuatorType::LPAREN);

    ifNode.condition = _parse_expression();

    _consume_punctuator(PunctuatorType::RPAREN);

    ifNode.body = _parse_statement();

    if (_match_keyword(KeywordType::ELSE))
    {
        _consume_keyword(KeywordType::ELSE);
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
    //TODO: Error, else without if
    Token head = _next();
    throw;
}

BLSL::Node_t BLSL::Parser::_parse_func()
{
    Token head = _next();
    ASTNode::Func funcNode;
    funcNode.debugPos = head.debugPos;

    auto identifierExpected = _get_identifier();
    if (!identifierExpected.has_value())
    {
        //TODO: THROW
        throw;
    }
    funcNode.identifier = identifierExpected.value();

    funcNode.returnSize = _consume_compile_time_size();

    _consume_punctuator(PunctuatorType::LPAREN);

    while (!_match_punctuator(PunctuatorType::RPAREN))
    {
        FormalParameter formalParam;
        auto identifierExpected = _get_identifier();
        if (!identifierExpected.has_value())
        {
            //TODO: Throw
            throw;
        }
        formalParam.identifier = identifierExpected.value();
        funcNode.parameters.push_back(formalParam);

        if (!_match_punctuator(PunctuatorType::RPAREN))_consume_punctuator(PunctuatorType::COMMA);
    }

    _consume_punctuator(PunctuatorType::RPAREN);

    auto compileSizes = _consume_compile_time_size_list();

    for (size_t i = 0; i < funcNode.parameters.size(); i++)
    {
        if (i == compileSizes.size()) throw;
        funcNode.parameters[i].size = compileSizes[i];
    }

    funcNode.body = _parse_statement();

    return std::make_unique<ASTNode::Func>(std::move(funcNode));
}

BLSL::Node_t BLSL::Parser::_parse_meminit()
{
    Token head = _next();
    ASTNode::MemInit memInitNode;
    memInitNode.debugPos = head.debugPos;

    memInitNode.size = _consume_compile_time_size();

    return std::make_unique<ASTNode::MemInit>(std::move(memInitNode));
}

BLSL::Node_t BLSL::Parser::_parse_alloc()
{
    Token head = _next();
    ASTNode::Alloc allocNode;
    allocNode.debugPos = head.debugPos;

    auto identifierExpected = _get_identifier();
    if (!identifierExpected.has_value())
    {
        //TODO throw
        throw;
    }

    allocNode.identifier = identifierExpected.value();
    allocNode.size = _consume_compile_time_size();

    return std::make_unique<ASTNode::Alloc>(std::move(allocNode));
}

BLSL::Node_t BLSL::Parser::_parse_expression(int lowestPrecedence)
{
    Node_t LHS;

    if (_peek().type == TokenType::PUNCTUATOR)
    {
        if (std::get<PunctuatorType>(_peek().subType) == PunctuatorType::LPAREN)
        {
            _next();

            LHS = _parse_expression();

            _consume_punctuator(PunctuatorType::RPAREN);
        }
        else
        {
            throw;
        }
    }
    else if (_peek().type == TokenType::OPERATOR)
    {
        if (PREFIX_PRECEDENCE.contains(std::get<OperatorType>(_peek().subType)))
        {
            auto token = _next();
            ASTNode::UnaryOperator unaryOperator;
            unaryOperator.debugPos = token.debugPos;
            unaryOperator.type = std::get<OperatorType>(token.subType);

            unaryOperator.right = _parse_expression(PREFIX_PRECEDENCE.at(std::get<OperatorType>(token.subType)));

            return std::make_unique<ASTNode::UnaryOperator>(std::move(unaryOperator));
        }
    }
    else
    {
        auto LHSExpected = _get_atom();
        if (!LHSExpected.has_value())
        {
            //TODO: Errors. std::print("{}", LHSExpected.error());
            throw;
        }

        LHS = std::move(LHSExpected.value());
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

        auto opExpected = _peek_operator();
        if (!opExpected.has_value()) break;

        auto [leftPrecedence, rightPrecedence] = OPERATOR_PRECEDENCE.at(opExpected.value());
        if (leftPrecedence < lowestPrecedence) break;

        auto op = _get_operator().value();

        auto RHS = _parse_expression(rightPrecedence);

        op->left = std::move(LHS);
        op->right = std::move(RHS);

        LHS = std::move(op);

    }

    return std::move(LHS);
}

BLSL::BodyNode_t BLSL::Parser::_parse_block()
{
    _consume_punctuator(PunctuatorType::LBRACE);
    ASTNode::BodyNode bodyNode;

    while (!_match_punctuator(PunctuatorType::RBRACE))
    {
        bodyNode.nodes.emplace_back(_parse_statement());
    }

    _consume_punctuator(PunctuatorType::RBRACE);

    return std::make_unique<ASTNode::BodyNode>(std::move(bodyNode));
}

BLSL::Node_t BLSL::Parser::_parse_statement()
{
    if (_match_punctuator(PunctuatorType::LBRACE))
    {
        BodyNode_t block = _parse_block();
        return block;
    }

    switch (_peek().type)
    {
        case TokenType::KEYWORD:
            switch (std::get<KeywordType>(_peek().subType))
            {
            case KeywordType::IDENTIFIER_PLACEHOLDER:
                //TODO THRow
                throw;

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
            throw;
            //TODO: Throw
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
        bodyNode.nodes.emplace_back(_parse_statement());
    }

    return std::make_unique<ASTNode::BodyNode>(std::move(bodyNode));

}
