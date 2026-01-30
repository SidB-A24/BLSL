//
// Created by Sid B on 30/01/26.
//

#include "../../headers/runtime/stack.h"

namespace BLSVM
{
    byte_t * Stack::get_ptr(const size_t elementIndex)
    {
        if (elementIndex >= _elementOffsets.size()) throw std::runtime_error("Stack Class Error Temporary"); //TODO THROW

        return _memory.data() + _elementOffsets[elementIndex].first;
    }

    size_t Stack::get_size(const size_t elementIndex) const
    {
        if (elementIndex >= _elementOffsets.size()) throw std::runtime_error("Stack Class Error Temporary"); //TODO THROW

        return _elementOffsets[elementIndex].second;
    }

    Stack::Stack()
        : _stackPointer(0)
    {
        _elementOffsets.reserve(OFFSET_VECTOR_SIZE);
    }

    void Stack::push(const byte_t *data, const size_t size)
    {
        if (_stackPointer + static_cast<ptrdiff_t>(size) > STACK_SIZE) throw std::runtime_error("Stack Class Error Temporary"); //TODO THROW

        memmove(_memory.data() + _stackPointer, data, size);

        _elementOffsets.emplace_back(_stackPointer, size);
        _stackPointer += static_cast<ptrdiff_t>(size);
    }

    void Stack::write(const byte_t *data, const size_t elementIndex)
    {
        if (elementIndex >= _elementOffsets.size()) throw std::runtime_error("Stack Class Error Temporary"); //TODO THROW

        memmove(_memory.data() + _elementOffsets[elementIndex].first, data, _elementOffsets[elementIndex].second);
    }

    void Stack::pop()
    {
        if (_elementOffsets.empty()) throw std::runtime_error("Stack Class Error Temporary"); //TODO THROW

        _stackPointer = _elementOffsets.back().first;
        _elementOffsets.pop_back();
    }
}
