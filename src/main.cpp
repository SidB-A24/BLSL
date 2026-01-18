#include <iostream>

#include <print>
#include <format>

#include "headers/core/blsl.h"
#include "headers/intermediate/lexer.h"
#include "headers/intermediate/parser.h"
#include "headers/log/log.h"

int main()
{
    BLSL::Lexer lexer;
    lexer.mount_source_from_file("../samples/expr.blsl");
    lexer.lex_to_file("../samples/out.blslex");

    lexer.mount_source_from_file("../samples/expr.blsl");
    auto lexOut = lexer.lex();


    BLSL::Parser parser(std::move(lexOut));

    BLSL::Node_t parseOut = parser.parse();

    BLSL::ASTNode::PrintVisitor visitor("../samples/out.blsparse");

    //std::print("AST:\n");

    parseOut->invite(visitor);

    std::cout << BLSL::Log::Mod::BLUE << BLSL::Log::TIME << BLSL::Log::Mod::CLEAR << std::endl;
    std::cout << BLSL::Log::box("Program End!", BLSL::Log::Mod::RED, BLSL::Log::Mod::MAGENTA, BLSL::Log::Mod::GREEN);
}

