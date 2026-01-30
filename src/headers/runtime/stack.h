//
// Created by Sid B on 30/01/26.
//

#include <array>
#include <vector>
#include "../core/blsbyc.h"

#ifndef BLSLANG_STACK_H
#define BLSLANG_STACK_H


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
        [[nodiscard]] byte_t* get_ptr(size_t elementIndex);
        [[nodiscard]] size_t get_size(size_t elementIndex) const;

    public:
        Stack();

        void push(const byte_t* data, size_t size);
        void write(const byte_t* data, size_t elementIndex);
        void pop();

    };
}
#endif //BLSLANG_STACK_H