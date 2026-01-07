#include <iostream>

#include <print>
#include <format>

#include "headers/core/blsl.h"

int main()
{
    BLSL::Token token;
    token.type = BLSL::TokenType::OPERATOR;
    token.subType = BLSL::OperatorType::ADD;
    token.debugPos = {12, 24};
    token.value = "Hello World!";
    std::print("{}", token);
}
