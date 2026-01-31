//
// Created by Sid B on 30/01/26.
//

#include "../../headers/runtime/mem.h"

namespace BLSVM
{
    inline byte_t * Stack::get_ptr(const size_t elementIndex)
    {
        if (elementIndex >= _elementOffsets.size()) throw std::runtime_error("Stack Class Error Temporary"); //TODO THROW

        return _memory.data() + _elementOffsets[elementIndex].first;
    }

    inline size_t Stack::get_size(const size_t elementIndex) const
    {
        if (elementIndex >= _elementOffsets.size()) throw std::runtime_error("Stack Class Error Temporary"); //TODO THROW

        return _elementOffsets[elementIndex].second;
    }

    Stack::Stack()
        : _stackPointer(0)
    {
        _elementOffsets.reserve(OFFSET_VECTOR_SIZE);
    }

    inline void Stack::push(const byte_t *data, const size_t size)
    {
        if (_stackPointer + static_cast<ptrdiff_t>(size) > STACK_SIZE) throw std::runtime_error("Stack Class Error Temporary"); //TODO THROW

        memmove(_memory.data() + _stackPointer, data, size);

        _elementOffsets.emplace_back(_stackPointer, size);
        _stackPointer += static_cast<ptrdiff_t>(size);
    }

    inline void Stack::write(const byte_t *data, const size_t elementIndex)
    {
        if (elementIndex >= _elementOffsets.size()) throw std::runtime_error("Stack Class Error Temporary"); //TODO THROW

        memmove(_memory.data() + _elementOffsets[elementIndex].first, data, _elementOffsets[elementIndex].second);
    }

    inline void Stack::pop()
    {
        if (_elementOffsets.empty()) throw std::runtime_error("Stack Class Error Temporary"); //TODO THROW

        _stackPointer = _elementOffsets.back().first;
        _elementOffsets.pop_back();
    }



    inline byte_t const * LiteralPool::read_data(size_t elementIndex) const
    {
        if (elementIndex >= _elementOffsets.size()) throw std::out_of_range("VM::read_data: invalid element index temporary"); //TODO THROW

        return const_cast<byte_t const*>(_memory.data() + _elementOffsets[elementIndex].first);
    }

    inline size_t LiteralPool::read_size(size_t elementIndex) const
    {
        if (elementIndex >= _elementOffsets.size()) throw std::out_of_range("VM::read_data: invalid element index temporary"); //TODO THROW

        return _elementOffsets[elementIndex].second;
    }

    LiteralPool::LiteralPool(std::istream& inputStream)
    {
        defer_init(inputStream);
    }


    void LiteralPool::defer_init(std::istream &inputStream)
    {
        ptrdiff_t  memoryOffset = 0;
        size_t literalCount;
        inputStream.read(reinterpret_cast<char*>(&literalCount), sizeof(literalCount));

        for (size_t i = 0; i < literalCount; i++)
        {
            size_t literalSize;
            if (!inputStream.read(reinterpret_cast<char*>(&literalSize), sizeof(literalSize))) throw; //TODO THROW

            _elementOffsets.emplace_back(memoryOffset, literalSize);

            _memory.resize(static_cast<size_t>(memoryOffset) + literalSize);

            if (!inputStream.read(reinterpret_cast<char*>(_memory.data() + memoryOffset), static_cast<std::streamsize>(literalSize))) throw; //TODO THROW

            memoryOffset += static_cast<ptrdiff_t>(literalSize);
        }
    }
}
