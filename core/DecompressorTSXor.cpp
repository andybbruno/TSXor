#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include "../lib/Zigzag.hpp"
#include "../lib/BitStream.cpp"
#include "../lib/Window.cpp"

struct DecompressorTSXor
{
    uint64_t nlines;
    uint64_t ncols;
    std::vector<uint8_t> bytes;
    std::vector<Window> window;
    std::vector<double> storedVal;
    uint64_t current_idx = 0;

    DecompressorTSXor(uint64_t lines, uint64_t cols, std::vector<uint8_t> &bts)
    {
        nlines = lines;
        ncols = cols;
        bytes = bts;
        window = std::vector<Window>(ncols);
        storedVal = std::vector<double>(ncols, 0);
    }

    bool hasNext()
    {
        return nlines > 0;
    }

    void next()
    {
        uint64_t final_val;
        uint64_t offset;
        uint64_t info;
        uint64_t trail_zeros_bytes;
        uint64_t xor_bytes;
        uint64_t xor_;
        uint64_t head;
        for (int i = 0; i < ncols; i++)
        {
            head = readBytes(1);

            if (head < 128)
            {
                final_val = window[i].get(head);
            }
            else if (head == 255)
            {
                final_val = readBytes(8);
            }
            else
            {
                offset = head & (~((UINT64_MAX << 7)));
                info = readBytes(1);
                trail_zeros_bytes = info >> 4;
                xor_bytes = info & (~((UINT64_MAX << 4)));
                xor_ = readBytes(xor_bytes) << (8 * trail_zeros_bytes);
                final_val = xor_ ^ window[i].get(offset);
            }
            window[i].insert(final_val);
            double p = (*(double *)&final_val);
            storedVal[i] = p;
        }

        nlines--;
    }

    inline uint64_t readBytes(size_t len)
    {
        uint64_t val = 0;
        for (int i = 0; i < len; i++)
        {
            val |= bytes[current_idx];
            current_idx++;
            if (i != (len - 1))
            {
                val <<= 8;
            }
        }
        return val;
    }
};
