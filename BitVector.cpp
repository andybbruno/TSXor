#include <vector>
#include <iostream>
#include <boost/dynamic_bitset.hpp>

using bitvec = std::vector<uint64_t>;

class BitVector
{
private:
public:
    bitvec data;
    uint64_t size = 0;
    uint64_t *curr_bucket;

    void append(uint64_t bits, uint64_t len)
    {
        assert(len == 64 || (bits >> len) == 0);

        uint64_t curr_bucket_pos = size % 64;
        uint64_t available_bits = 64 - curr_bucket_pos;
        size += len;

        if (curr_bucket_pos == 0)
        {
            data.push_back(bits << (64 - len));
        }
        else
        {
            if (len <= available_bits)
            {
                *curr_bucket ^= bits << (available_bits - len);
            }
            else
            {
                *curr_bucket ^= bits >> (len - available_bits);
                data.push_back(bits << (64 - len + available_bits));
            }
        }
        curr_bucket = &data.back();
    }

    // void push_back(bool b)
    // {
    // }

    // size_t size()
    // {
    // }
};
