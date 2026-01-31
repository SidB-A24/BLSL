//
// Created by Sid B on 06/01/26.
//

#include "../../headers/runtime/vm.h"

#include <fstream>

namespace BLSVM
{
    const std::vector<RegisterInfo> REGISTER_INFO = {
        {
            RegisterType::INTERNAL,
            RegisterFlag::REG_FLAG_SPECIAL_WRITEABLE,
            16
        },
        {
            RegisterType::ARGS,
            RegisterFlag::REG_FLAG_PRE_CALL_WRITABLE,
            32
        },
        {
            RegisterType::RETURNS,
            RegisterFlag::REG_FLAG_POST_CALL_WRITEABLE,
            32
        },
        {
            RegisterType::SCI,
            RegisterFlag::REG_FLAG_WRITABLE,
            64
        },
        {
            RegisterType::GENERAL,
            RegisterFlag::REG_FLAG_WRITABLE,
            112
        }
    };

    inline reginfo_t make_reginfo(regtype_t type, regflag_t flags)
    {
        reginfo_t reginfo = 0;

        reginfo |= (type << REGISTER_TYPE_SHIFT) & REGISTER_TYPE_MASK;
        reginfo |= (flags << REGISTER_FLAG_SHIFT) & REGISTER_FLAG_MASK;

        return reginfo;
    }

    VM::VM()
    {
        size_t i = 0;
        for (auto info: REGISTER_INFO)
        {
            for (size_t j = 0; j < info.count && i < REGISTER_COUNT; j++, i++)
            {
                _registers[i].info = make_reginfo(static_cast<regtype_t>(info.type), j);
            }
        }
    }
}
