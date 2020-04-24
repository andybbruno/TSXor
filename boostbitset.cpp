#include <boost/dynamic_bitset.hpp>
#include <iostream>

void append(boost::dynamic_bitset<uint64_t> &set, uint64_t value, size_t nbits)
{
    set.resize(set.size() + nbits);
    for (size_t i = 1; i <= nbits; i++)
    {
        set[set.size() - i] = value & 1;
        value >>= 1;
    }
}

int main()
{
    boost::dynamic_bitset<uint64_t> bset;
    bset.append(5);
    append(bset, 5, 4);
    append(bset, 5, 4);

    std::cout << bset << std::endl;

    return 0;
}