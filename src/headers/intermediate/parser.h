//
// Created by Sid B on 06/01/26.
//

#ifndef BLSLANG_PARSER_H
#define BLSLANG_PARSER_H

#include <expected>

#include "astnodes.h"
#include "../err/err.h"

namespace BLSL
{
    class Parser
    {
    private:
        std::unique_ptr<std::vector<Token>> _tokens;
        size_t _pos;

    private:
        Token _peek() const;
        Token _next();


        [[deprecated]] std::expected<OperatorType, Token> _peek_operator() const;

        [[deprecated]] std::expected<Node_t, Token> _get_atom();
        [[deprecated]] std::expected<std::string, Token> _get_identifier();
        [[deprecated]] std::expected<Token, Token> _get_literal();
        [[deprecated]] std::expected<std::unique_ptr<ASTNode::BinaryOperator>, Token> _get_operator();

        [[deprecated]] bool _match_punctuator(PunctuatorType pType) const;
        [[deprecated]] bool _match_comparator(ComparatorType cType) const;
        [[deprecated]] bool _match_keyword(KeywordType kType) const;

        [[deprecated]] void _consume_punctuator(PunctuatorType pType);
        [[deprecated]] void _consume_operator(OperatorType oType);
        [[deprecated]] void _consume_comparator(ComparatorType cType);
        [[deprecated]] void _consume_keyword(KeywordType kType);

        size_t _consume_compile_time_size();
        std::vector<size_t> _consume_compile_time_size_list();

    private:
        Node_t _get_atom_consume();
        std::string _get_identifier_consume();
        Token _get_literal_consume();
        std::unique_ptr<ASTNode::BinaryOperator> _get_operator_consume();

        template <BLSLEnum EnumTy>
        bool _match(EnumTy required)
        {

        }

        template <BLSLEnum EnumTy>
        bool _consume(EnumTy required)
        {

        }


    private:

        Node_t _parse_for();
        Node_t _parse_while();
        Node_t _parse_if();
        Node_t _parse_else();

        Node_t _parse_func();
        Node_t _parse_meminit();
        Node_t _parse_alloc();

        Node_t _parse_expression(int lowestPrecedence = -1);
        BodyNode_t _parse_block();
        Node_t _parse_statement();


    public:
        explicit Parser(std::unique_ptr<std::vector<Token>> in);

        Node_t parse();
    };
}

#endif //BLSLANG_PARSER_H