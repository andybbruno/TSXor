#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include "BitStream.cpp"
#include "Window.cpp"
#include "zigzag.hpp"

#define DELTA_7_MASK 0x02 << 7;
#define DELTA_9_MASK 0x06 << 9;
#define DELTA_12_MASK 0x0E << 12;

struct CompressorLZXOR
{
    uint countA = 0;
    uint countB = 0;
    uint countC = 0;
    uint countB_bytes = 0;
    
    uint8_t FIRST_DELTA_BITS = 32;
    long storedTimestamp = 0;
    long storedDelta = 0;
    long blockTimestamp = 0;
    
    std::vector<Window> window;

    BitStream out;
    std::vector<uint8_t> bytes;

    CompressorLZXOR(uint64_t timestamp)
    {
        blockTimestamp = timestamp;
        addHeader(timestamp);
    }

    void addHeader(uint64_t timestamp)
    {
        out.append(timestamp, 64);
    }

    void addValue(uint64_t timestamp, std::vector<double> const &vals)
    {
        if (storedTimestamp == 0)
        {
            window = std::vector<Window>(vals.size());
            writeFirst(timestamp, vals);
        }
        else
        {
            compressTimestamp(timestamp);
            compressValue(vals);
        }
    }

    void writeFirst(uint64_t timestamp, std::vector<double> const &values)
    {
        storedDelta = timestamp - blockTimestamp;
        storedTimestamp = timestamp;

        out.append(storedDelta, FIRST_DELTA_BITS);
        for (int i = 0; i < values.size(); i++)
        {
            uint64_t x = *((uint64_t *)&(values[i]));
            append64(x);
            window[i].insert(x);
        }
    }

    void close()
    {
        out.close();
    }

    void compressTimestamp(long timestamp)
    {
        // a) Calculate the delta of delta
        int64_t newDelta = (timestamp - storedTimestamp);
        int64_t deltaD = newDelta - storedDelta;

        if (deltaD == 0)
        {
            out.push_back(0);
        }
        else
        {
            deltaD = zz::encode(deltaD);
            auto length = 64 - __builtin_clzll(deltaD);

            switch (length)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                //DELTA_7_MASK adds '10' to deltaD
                deltaD |= DELTA_7_MASK;
                out.append(deltaD, 9);
                break;
            case 8:
            case 9:
                //DELTA_9_MASK adds '110' to deltaD
                deltaD |= DELTA_9_MASK;
                out.append(deltaD, 12);
                break;
            case 10:
            case 11:
            case 12:
                //DELTA_12_MASK adds '1110' to deltaD
                deltaD |= DELTA_12_MASK;
                out.append(deltaD, 16);
                break;
            default:
                // Append '1111'
                out.append(0x0F, 4);
                out.append(deltaD, 32);
                // out.append(deltaD, 64);
                break;
            }
        }

        storedDelta = newDelta;
        storedTimestamp = timestamp;
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

                countA++;
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

                    countB++;
                    countB_bytes += (2 + xor_len_bytes);
                }
                else
                {
                    append8((uint8_t)255);
                    append64(val);

                    countC++;
                }
            }
        }

        for (int i = 0; i < values.size(); i++)
        {
            uint64_t val = *((uint64_t *)&values[i]);
            window[i].insert(val);
        }
    }

    inline void append64(uint64_t x)
    {
        uint8_t *b = (uint8_t *)&x;
        for (int i = 7; i >= 0; i--)
        {
            bytes.push_back(b[i]);
        }
    }

    inline void append8(uint8_t x)
    {
        bytes.push_back(x);
    }
};
