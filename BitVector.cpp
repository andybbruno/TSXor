#pragma once
#include <vector>
// #include <iostream>
// #include <boost/dynamic_bitset.hpp>

class BitVector
{
private:
    std::vector<uint64_t> data;
    uint64_t m_size = 0;
    uint64_t *curr_bucket;

public:
    void reserve(uint64_t n)
    {
        data.reserve(n);
    }

    inline void append(uint64_t bits, uint64_t len)
    {
        assert(len == 64 || (bits >> len) == 0);

        uint64_t curr_bucket_pos = m_size % 64;
        uint64_t available_bits = 64 - curr_bucket_pos;
        m_size += len;

        if (curr_bucket_pos == 0)
        {
            data.push_back(bits << (64 - len));
        }
        else if (curr_bucket_pos <= 64 - len)
        {
            *curr_bucket ^= bits << (available_bits - len);
        }
        else
        {
            *curr_bucket ^= bits >> (len - available_bits);
            data.push_back(bits << (64 - len + available_bits));
        }
        curr_bucket = &data.back();

        // if (curr_bucket_pos == 0)
        // {
        //     data.push_back(bits << (64 - len));
        // }
        // else
        // {
        //     if (len <= available_bits)
        //     {
        //         *curr_bucket ^= bits << (available_bits - len);
        //     }
        //     else
        //     {
        //         *curr_bucket ^= bits >> (len - available_bits);
        //         data.push_back(bits << (64 - len + available_bits));
        //     }
        // }
        // curr_bucket = &data.back();
    }

    void push_back(bool b)
    {
        append(b, 1);
    }

    size_t size()
    {
        return m_size;
    }

    friend std::ostream &operator<<(std::ostream &out, const BitVector &b)
    {
        for (auto v : b.data)
        {
            out << std::bitset<64>(v);
        }
        return out;
    }
};

// int main()
// {
//     BitVector b;
//     b.append(1, 1);
//     b.append(3, 3);
//     b.append(1, 60);
//     b.append(7, 4);

//     std::cout << b << std::endl;
// }