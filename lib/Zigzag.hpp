#pragma once
#include <utility>

namespace zz
{
    uint64_t encode(int64_t i)
    {
        return (i >> 63) ^ (i << 1);
    }

    int64_t decode(uint64_t i)
    {
        return (i >> 1) ^ (-(i & 1));
    }
} // namespace zz