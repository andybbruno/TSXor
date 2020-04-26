// #include <boost/dynamic_bitset.hpp>
#include <vector>
struct bitset
{
    size_t pos = 0;
    std::vector<bool> set;

    void writeBits(uint64_t value, size_t nbits)
    {
        auto s = set.size();
        set.resize(s + nbits);
        for (size_t i = 1; i <= nbits; i++)
        {
            set[s - i] = value & 1;
            value >>= 1;
        }
    }

    void push_back(bool b)
    {
        set.push_back(b);
    }

    size_t size()
    {
        return set.size();
    }

    uint64_t get(size_t len)
    {
        return x;
    }
};