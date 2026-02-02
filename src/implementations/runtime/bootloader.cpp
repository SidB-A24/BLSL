//
// Created by Sid B on 02/02/26.
//

#include "../../headers/runtime/bootloader.h"
BLSVM::Bootloader::Bootloader(std::istream &in)
: _inFile(in)
{
    _vm = std::make_unique<VM>();
}

void BLSVM::Bootloader::load() const
{
    Bytecode::section_t section;

    while (_inFile.read(reinterpret_cast<char*>(&section), sizeof(Bytecode::section_t)))
    {
        switch (static_cast<Bytecode::Section>(section))
        {
            case Bytecode::Section::COMPILE_TIME_SIZES:
                _vm->defer_load_csz(_inFile);
            case Bytecode::Section::LITERALS:
                _vm->defer_load_lp(_inFile);
            case Bytecode::Section::INSTRUCTIONS:
                _vm->defer_load_vm(_inFile);
        }
    }

}

void BLSVM::Bootloader::boot() const
{
    _vm->boot();
}
