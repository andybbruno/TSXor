#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include "../lib/Window.cpp"

struct CompressorTSXor
{
    std::vector<double> storedValues;
    std::vector<uint8_t> bytes;
    std::vector<Window> window;
    long nlines = 0;

    CompressorTSXor(std::vector<double> const &values)
    {
        window = std::vector<Window>(values.size());

        for (int i = 0; i < values.size(); i++)
        {
            uint64_t x = *((uint64_t *)&(values[i]));
            append64(x);
            window[i].insert(x);
        }
    }

    void addValue(std::vector<double> const &vals)
    {
        compressValue(vals);
        nlines++;
    }

    void close()
    {
        append64(0);
    }

    void compressValue(std::vector<double> const &values)
    {
        for (int i = 0; i < values.size(); i++)
        {
            uint64_t val = *((uint64_t *)&values[i]);

            if (window[i].contains(val))
            {
                auto offset = window[i].getIndexOf(val);
                uint8_t *bytes = (uint8_t *)&offset;
                append8(bytes[0]);
            }
            else
            {
                uint64_t candidate = window[i].getCandidate(val);

                uint64_t xor_ = candidate ^ val;

                int lead_zeros_bytes = (__builtin_clzll(xor_) / 8);
                int trail_zeros_bytes = (__builtin_ctzll(xor_) / 8);

                if ((lead_zeros_bytes + trail_zeros_bytes) > 1)
                {
                    auto offset = window[i].getIndexOf(candidate);

                    //WRITE 1
                    offset |= 0x80;
                    uint8_t *bytes = (uint8_t *)&offset;
                    append8(bytes[0]);

                    auto xor_len_bytes = 8 - lead_zeros_bytes - trail_zeros_bytes;
                    xor_ >>= (trail_zeros_bytes * 8);

                    uint8_t head = (trail_zeros_bytes << 4) | xor_len_bytes;
                    append8(head);

                    uint8_t *xor_bytes = (uint8_t *)&xor_;
                    for (int i = (xor_len_bytes - 1); i >= 0; i--)
                    {
                        append8(xor_bytes[i]);
                    }
                }
                else
                {
                    append8((uint8_t)255);
                    append64(val);
                }
            }
        }

        for (int i = 0; i < values.size(); i++)
        {
            uint64_t val = *((uint64_t *)&values[i]);
            window[i].insert(val);
        }
    }

    void append64(uint64_t x)
    {
        uint8_t *b = (uint8_t *)&x;
        for (int i = 7; i >= 0; i--)
        {
            bytes.push_back(b[i]);
        }
    }

    void append8(uint8_t x)
    {
        bytes.push_back(x);
    }
};