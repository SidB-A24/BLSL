//
// Created by Sid B on 01/02/26.
//

#include "../../headers/debug/debug.h"

void DEBUG::write_instruction_to_stream(BLSVM::Bytecode::instruction_t instruction, std::ostream& output)
{
    output.write(reinterpret_cast<const char*>(&instruction), sizeof(BLSVM::Bytecode::instruction_t));
}

void DEBUG::write_literal_to_stream(size_t size, BLSVM::ubyte_t *data, std::ostream& output)
{
    output.write(reinterpret_cast<char*>(&size), sizeof(size_t));
    for (size_t i = 0; i < size; i++)
    {
        output.write(reinterpret_cast<char*>(&data[i]), sizeof(BLSVM::ubyte_t));
    }
}

void DEBUG::write_section_to_stream(BLSVM::Bytecode::section_t section, std::ostream& output)
{
    output.write(reinterpret_cast<char*>(&section), sizeof(BLSVM::Bytecode::section_t));
}
