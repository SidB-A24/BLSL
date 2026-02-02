#include <iostream>

#include <print>
#include <format>

#include "headers/core/blsl.h"
#include "headers/intermediate/lexer.h"
#include "headers/intermediate/parser.h"
#include "headers/log/log.h"

#include "headers/debug/debug.h"

int main()
{
    /*
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
*/

    std::ofstream bytecodeFile("../samples/out.blsbyc", std::ios::binary);

    BLSVM::Bytecode::section_t section;

    section = static_cast<BLSVM::Bytecode::section_t>(BLSVM::Bytecode::Section::COMPILE_TIME_SIZES);
    DEBUG::write_section_to_stream(section, bytecodeFile);

    size_t literalCount = 1;
    bytecodeFile.write(reinterpret_cast<char*>(&literalCount), sizeof(size_t));

    size_t compileTimeSize = 16; //16 byte result buffer
    bytecodeFile.write(reinterpret_cast<char*>(&compileTimeSize), sizeof(size_t));

    section = static_cast<BLSVM::Bytecode::section_t>(BLSVM::Bytecode::Section::LITERALS);
    DEBUG::write_section_to_stream(section, bytecodeFile);

    literalCount = 2;
    bytecodeFile.write(reinterpret_cast<char*>(&literalCount), sizeof(size_t));

    BLSVM::ubyte_t data1[2] = {0xFF, 0xFF};
    BLSVM::ubyte_t data2[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    DEBUG::write_literal_to_stream(2, data1, bytecodeFile);
    DEBUG::write_literal_to_stream(4, data2, bytecodeFile);

    section = static_cast<BLSVM::Bytecode::section_t>(BLSVM::Bytecode::Section::INSTRUCTIONS);
    DEBUG::write_section_to_stream(section, bytecodeFile);

    //Allocating result buffer

    BLSVM::Bytecode::opcode_t opcode = static_cast<BLSVM::Bytecode::opcode_t>(BLSVM::Bytecode::OpCode::ALLOC_STACK);

    BLSVM::Bytecode::operand_t operandA = 0b1000'0000'0000'0001;
    BLSVM::Bytecode::operand_t operandB = 0b1000'0000'0000'0000;
    BLSVM::Bytecode::operand_t operandC = 0b0000'0000'1000'0000;

    BLSVM::Bytecode::Instruction instruction = BLSVM::Bytecode::decode_instruction(BLSVM::Bytecode::make_instruction(
        opcode, operandC, operandB
    ));

    DEBUG::write_instruction_to_stream(instruction, bytecodeFile);

    //Clinging result buffer to register 128

    opcode = static_cast<BLSVM::Bytecode::opcode_t>(BLSVM::Bytecode::OpCode::CLING_STACK);

    instruction = BLSVM::Bytecode::decode_instruction(BLSVM::Bytecode::make_instruction(
        opcode, operandC
    ));

    DEBUG::write_instruction_to_stream(instruction, bytecodeFile);


    //Adding the two numbers into result buffer in register 128

    opcode = static_cast<BLSVM::Bytecode::opcode_t>(BLSVM::Bytecode::OpCode::UNSIGNED_ADD);

    instruction = BLSVM::Bytecode::decode_instruction(BLSVM::Bytecode::make_instruction(
        opcode, operandA, operandB, operandC
    ));
    DEBUG::write_instruction_to_stream(instruction, bytecodeFile);

    //Dumping the result buffer to standard out

    opcode = static_cast<BLSVM::Bytecode::opcode_t>(BLSVM::Bytecode::OpCode::DEBUG_DUMP);

    instruction = BLSVM::Bytecode::decode_instruction(BLSVM::Bytecode::make_instruction(
        opcode, operandC
    ));
    DEBUG::write_instruction_to_stream(instruction, bytecodeFile);

    bytecodeFile.close();
}

