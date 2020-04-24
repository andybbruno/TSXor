#include <boost/dynamic_bitset.hpp>
#include <vector>
struct bitset
{
    boost::dynamic_bitset<> set;
    // std::vector<bool> set;

    void writeBits(uint64_t value, size_t nbits)
    {
        set.resize(set.size() + nbits);
        for (size_t i = 1; i <= nbits; i++)
        {
            set[set.size() - i] = value & 1;
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
};