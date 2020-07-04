#pragma once
#include <utility>

namespace zz
{
    inline uint32_t encode(int32_t i)
    {
        if (i < 0)
            return (i * (-2)) - 1;
        else
            return i * 2;
    }

    inline int32_t decode(uint32_t i)
    {
        if ((i % 2) == 0)
            return (i / 2);
        else
            return (i + 1) / (-2);
    }
} // namespace zz
