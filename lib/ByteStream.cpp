#pragma once
#include <deque>
#include <iostream>
#include <cassert>
#include <fstream>

class ByteStream
{
private:
    std::deque<uint8_t> data;

    // False = you can only write
    // True = you can only read
    bool closed = false;

public:
    void append(uint8_t bits)
    {
        data.push_back(bits);
    }

    void append(uint64_t bits, uint64_t len)
    {
        // std::cout << bits << " ::: " << len << std::endl;
        assert(!closed);
        assert((len % 8) == 0);
        assert(len == 64 || (bits >> len) == 0);

        int numbytes = len / 8;

        uint8_t *bytes = (uint8_t *)&bits;

        for (int i = 0; i < numbytes; i++)
        {
            auto idx = numbytes - i - 1;
            // std::cout << +bytes[idx] << std::endl;
            data.push_back(bytes[idx]);
        }
    }

    void close()
    {
        append(0x0F);
        append(UINT64_MAX, 64);
        append(UINT64_MAX, 64);
        closed = true;
    }

    size_t size()
    {
        return data.size() * 8;
    }

    uint64_t get(uint64_t len)
    {
        assert(closed);
        assert(len <= 64);
        assert(data.size() > 0);

        uint64_t t_bits = 0;

        return t_bits;
    }
};