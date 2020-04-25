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
    uint64_t curr_bits = 64;

    //false = read only
    //true = write only
    bool rw = false;

public:
    void reserve(uint64_t n)
    {
        assert(!rw);
        data.reserve(n);
    }

    inline void append(uint64_t bits, uint64_t len)
    {
        assert(!rw);
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

    void close()
    {
        append(0x0F, 4);
        append(0xFFFFFFFF, 32);
        push_back(0);
        rw = true;
        curr_bucket = &data.front();
    }

    void push_back(bool b)
    {
        assert(!rw);
        append(b, 1);
    }

    size_t size()
    {
        return m_size;
    }

    friend std::ostream &operator<<(std::ostream &out, const BitVector &b)
    {
        // std::bitset<64> tmp(b.data[0] >> (64 - b.curr_bits));
        std::bitset<64> tmp(b.data[0]);

        for (int i = 0; i < (b.curr_bits); i++)
        {
            auto pos = 63 - i;
            out << tmp[pos];
        }
        for (int i = b.curr_bits; i < 64; i++)
        {
            out << "#";
        }
        out << "\n";
        if (b.data.size() > 1)
        {
            for (int i = 1; i < b.data.size(); i++)
            {
                out << std::bitset<64>(b.data[i]) << "\n";
            }
        }
        return out << "\n";
    }

    uint64_t get(uint64_t len)
    {
        assert(rw);
        assert(len <= 64);
        assert(data.size() > 0);

        if (len == curr_bits)
        {
            auto t_bits = *curr_bucket;
            data.erase(data.begin());
            curr_bucket = &data.front();
            curr_bits = 64;
            return t_bits;
        }
        else if (len < curr_bits)
        {
            auto t_bits = *curr_bucket >> (64 - len);
            *curr_bucket <<= len;
            curr_bits -= len;
            return t_bits;
        }
        else
        {
            auto rest = len - curr_bits;
            auto t_bits = *curr_bucket;
            data.erase(data.begin());
            curr_bucket = &data.front();

            auto a = (t_bits >> (64 - len));
            auto b = (*curr_bucket >> (64 - rest));
            t_bits = a ^ b;

            *curr_bucket <<= rest;
            curr_bits = 64 - rest;
            return t_bits;
        }
    }
};

// int main()
// {
//     BitVector b;
//     b.append(UINT64_MAX, 64);
//     b.append(0xC000000000000001, 64);
//     b.append(UINT64_MAX, 64);
//     b.append(0xC000000000000001, 64);
//     // b.append(1, 60);
//     // b.append(7, 4);
//     b.close();

//     std::cout << b.get(1) << std::endl;
//     std::cout << b << std::endl;
//     std::cout << b.get(1) << std::endl;
//     std::cout << b << std::endl;
//     std::cout << b.get(60) << std::endl;
//     std::cout << b << std::endl;
//     std::cout << b.get(6) << std::endl;
//     std::cout << b << std::endl;
// }
