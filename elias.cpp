#include <iostream>
#include <vector>
#include <boost/dynamic_bitset.hpp>

// computes the Elias Gamma encoding of x
//
// return < length , gamma(x) >
//
std::pair<uint64_t, uint64_t> gammaEncoder(uint64_t x)
{
    uint64_t len_x = 64 - __builtin_clzll(x);
    return std::make_pair((2 * len_x) - 1, x);
}

// computes the Elias Delta encoding of x
//
// return < length , delta(x) >
//
std::pair<uint64_t, uint64_t> deltaEncoder(uint64_t x)
{
    if (x == 1)
        return std::make_pair(1, 1);
    else
    {
        uint64_t len_x = 64 - __builtin_clzll(x);
        auto gam = gammaEncoder(len_x);
        gam.first <<= len_x - 1;
        x <<= 64 - len_x + 1;
        x >>= 64 - len_x + 1;
        x ^= gam.first;
        return std::make_pair(gam.second + len_x - 1, x);
    }
}

int main()
{
    std::vector<boost::dynamic_bitset<>> list;
    for (uint64_t i = 1; i < 20; i++)
    {
        auto a = deltaEncoder(i);
        list.push_back(boost::dynamic_bitset<>(a.second, a.first));
    }
    for (auto x : list)
    {
        std::cout << x << std::endl;
    }
    return 0;
}