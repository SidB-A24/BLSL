#include <iostream>

#include <print>
#include <format>

#include "headers/core/blsl.h"
#include "headers/intermediate/lexer.h"
#include "headers/intermediate/parser.h"

int main()
{
    BLSL::Lexer lexer;
    lexer.mount_source_from_file("../samples/expr.blsl");
    auto lexOut = lexer.lex();

    //lexer.lex_to_file("../samples/out.blslex");

    BLSL::Parser parser(std::move(lexOut));

    BLSL::Node_t parseOut = parser.parse();

    BLSL::ASTNode::ConsoleVisitor visitor;

    std::print("AST:");

    parseOut->invite(visitor);

    std::print("Timeout");
}

