//
// Created by Sid B on 01/02/26.
//

#ifndef BLSLANG_DEBUG_H
#define BLSLANG_DEBUG_H
#include <iostream>
#include <iomanip>
#include "../core/blsbyc.h"

namespace DEBUG
{
    void write_instruction_to_stream(BLSVM::Bytecode::instruction_t instruction, std::ostream& output);
    void write_literal_to_stream(size_t size, BLSVM::ubyte_t* data, std::ostream& output);
    void write_section_to_stream(BLSVM::Bytecode::section_t section, std::ostream& output);
}

#endif //BLSLANG_DEBUG_H