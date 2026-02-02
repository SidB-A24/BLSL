//
// Created by Sid B on 02/02/26.
//

#ifndef BLSLANG_BOOTLOADER_H
#define BLSLANG_BOOTLOADER_H
#include <iostream>
#include <memory>

#include "../core/blsbyc.h"
#include "vm.h"

namespace BLSVM
{
    class Bootloader
    {
    private:
        std::istream& _inFile;
        std::unique_ptr<VM> _vm;
    public:
        explicit Bootloader(std::istream& in);

        void load() const;
        void boot() const;
    };
}

#endif //BLSLANG_BOOTLOADER_H