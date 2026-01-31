//
// Created by Sid B on 30/01/26.
//


#ifndef BLSLANG_STACK_H
#define BLSLANG_STACK_H

#include <array>
#include <vector>
#include <iostream>
#include "../core/blsbyc.h"

namespace BLSVM
{
    inline constexpr size_t STACK_SIZE = 1024*1024*4;
    inline constexpr size_t OFFSET_VECTOR_SIZE = 1024*4;

    class Stack
    {
    private:
        std::array<byte_t, STACK_SIZE> _memory{};
        std::vector<std::pair<ptrdiff_t, size_t>> _elementOffsets;                  //Stores the offsets of elements within the stack and stores their sizes.
        ptrdiff_t _stackPointer;

    protected:
        [[nodiscard]] inline byte_t* get_ptr(size_t elementIndex);
        [[nodiscard]] inline size_t get_size(size_t elementIndex) const;

    public:
        Stack();

        inline void push(const byte_t* data, size_t size);
        inline void write(const byte_t* data, size_t elementIndex);
        inline void pop();

    };
}

namespace BLSVM
{
    class LiteralPool
    {
    private:
        std::vector<byte_t> _memory;
        std::vector<std::pair<ptrdiff_t, size_t>> _elementOffsets;

    protected:
        [[nodiscard]] inline byte_t const* read_data(size_t elementIndex) const;
        [[nodiscard]] inline size_t read_size(size_t elementIndex) const;

    public:
        explicit LiteralPool(std::istream& inputStream);
        LiteralPool() = default;

        void defer_init(std::istream& inputStream);
    };
}

#endif //BLSLANG_STACK_H