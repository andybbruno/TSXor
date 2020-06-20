#pragma once
#include <utility>

namespace elias
{
using Pair = std::pair<uint64_t, uint64_t>;

// computes the Elias Gamma encoding of x
//
// return < gamma(x) , length >
//
inline Pair gamma(uint64_t x)
{
    uint64_t len_x = 64 - __builtin_clzll(x);
    return Pair(x, (2 * len_x) - 1);
}

// computes the Elias Delta encoding of x
//
// return < delta(x) , length >
//
inline Pair delta(uint64_t x)
{
    if (x == 1)
        return Pair(1, 1);
    else
    {
        uint64_t len_x = 64 - __builtin_clzll(x);
        auto gam = gamma(len_x);
        gam.first <<= len_x - 1;
        x <<= 64 - len_x + 1;
        x >>= 64 - len_x + 1;
        x ^= gam.first;
        return Pair(x, gam.second + len_x - 1);
    }
}
} // namespace elias