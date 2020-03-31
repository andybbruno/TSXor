#include "BitStream.cpp"
#include "DataPoint.cpp"
#include "succinct/bit_vector.hpp"
#include <string>
#include <fstream>
#include <iostream>
struct Compressor
{
    // DataPoint header;

    std::fstream outfile;

    bool FIRST_BLOCK = true;

    // std::vector<double> curr;
    // std::vector<double> curr_xorWithPrev;
    // std::vector<double> prev;
    // std::vector<double> prev_xorWithPrev;

    std::vector<double> last_xorWithPrev;
    std::vector<double> last_vals;
    uint64_t last_time;
    uint64_t sec_last_time;

    uint64_t head_time;
    std::vector<double> head_vals;

    succinct::bit_vector_builder bs;

    int num_cols;

    Compressor(uint64_t time, std::vector<double> const &vals) : head_time(time), head_vals(vals)
    {
        num_cols = vals.size();
        last_vals = std::vector<double>(num_cols, 0);
        last_xorWithPrev = std::vector<double>(num_cols, 0);

        bs.append_bits(time, 64);

        for (auto v : vals)
            bs.append_bits(v, 64);
    };

    void deltaEncoding(int64_t delta)
    {
        if (delta == 0)
        {
            bs.push_back(0);
        }
        else if ((-63 < delta) && (delta < 64))
        {
            delta += 63;
            bs.push_back(1);
            bs.push_back(0);
            bs.append_bits(delta, 7);
        }
        else if ((-255 < delta) && (delta < 256))
        {
            delta += 255;
            bs.push_back(1);
            bs.push_back(1);
            bs.push_back(0);
            bs.append_bits(delta, 9);
        }
        else if ((-2047 < delta) && (delta < 2048))
        {
            delta += 2047;
            bs.push_back(1);
            bs.push_back(1);
            bs.push_back(1);
            bs.push_back(0);
            bs.append_bits(delta, 12);
        }
        else
        {
            if (delta < 0)
            {
                delta ^= 0xFFFFFFFF00000000;
            }

            bs.push_back(1);
            bs.push_back(1);
            bs.push_back(1);
            bs.push_back(1);

            bs.append_bits(delta, 32);
        }
    }

    void valuesEncoding(std::vector<double> const &actual)
    {
        std::vector<double> actual_xorWithPrev(actual.size());

        for (int i = 0; i < actual.size(); i++)
        {
            // auto current = (uint64_t *)&actual[i];
            // auto past = (uint64_t *)&last_vals[i];
            // uint64_t xored = *current ^ *past;

            uint64_t *a = (uint64_t *)&actual[i];
            uint64_t *b = (uint64_t *)&last_vals[i];
            uint64_t xored = *a ^ *b;
            actual_xorWithPrev[i] = xored;

            if (xored == 0)
            {
                bs.push_back(0);
            }
            else
            {
                // count leading and traling zeros
                uint64_t leadingZeros = __builtin_clzll(xored);
                uint64_t trailingZeros = __builtin_ctzll(xored);

                uint64_t prevXorLeadingZeros = __builtin_clzll(last_xorWithPrev[i]);
                uint64_t prevXorTrailingZeros = __builtin_ctzll(last_xorWithPrev[i]);

                uint64_t corePart = xored >> trailingZeros;
                int coreLength = 64 - leadingZeros - trailingZeros;

                if ((leadingZeros == prevXorLeadingZeros) && (trailingZeros == prevXorTrailingZeros))
                {
                    // '1' + control bit '0'
                    bs.push_back(1);
                    bs.push_back(0);
                }
                else
                {
                    // '1' + control bit '1'
                    bs.push_back(1);
                    bs.push_back(1);

                    //  5 bits for leading zeros
                    bs.append_bits(leadingZeros, 5);

                    //  6 bits for the length of the core part of the XOR
                    bs.append_bits(coreLength, 6);
                }

                bs.append_bits(corePart, coreLength);
            }
        }
    }

    void append(uint64_t ts, std::vector<double> const &values)
    {
        if (FIRST_BLOCK)
        {
            auto delta = ts - head_time;
            bs.append_bits(delta, 14);
            valuesEncoding(values);

            sec_last_time = head_time;
            last_time = ts;
            last_vals = values;
            FIRST_BLOCK = false;
        }
        else
        {
            int64_t delta = (ts - last_time) - (last_time - sec_last_time);

            deltaEncoding(delta);
            valuesEncoding(values);

            sec_last_time = last_time;
            last_time = ts;
            last_vals = values;
        }
    }
};