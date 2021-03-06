#include <vector>
#include <map>
#include <string>
#include "lib/Zigzag.hpp"
#include "lib/BitStream.cpp"

#define DELTA_7_MASK 0x02 << 7;
#define DELTA_9_MASK 0x06 << 9;
#define DELTA_12_MASK 0x0E << 12;

struct CompressorGorilla
{
    std::vector<uint64_t> storedLeadingZeros;
    std::vector<uint64_t> storedTrailingZeros;
    std::vector<double> storedValues;

    long storedTimestamp = 0;
    long storedDelta = 0;

    BitStream bs_times;
    BitStream bs_values;

    // uint countA = 0;
    // uint countB = 0;
    // uint countC = 0;
    // std::vector<uint64_t> countB_vec;
    // std::vector<uint64_t> countC_vec;


    CompressorGorilla(uint64_t timestamp, std::vector<double> const &values)
    {
        bs_times.append(timestamp, 64);

        for (double d : values)
        {
            uint64_t *x = (uint64_t *)&d;
            bs_values.append(*x, 64);
        }

        storedTimestamp = timestamp;
        storedValues = values;
        storedLeadingZeros = std::vector<uint64_t>(values.size(), 0);
        storedTrailingZeros = std::vector<uint64_t>(values.size(), 64);
    }

    void addValue(uint64_t timestamp, std::vector<double> const &vals)
    {
        compressTimestamp(timestamp);
        compressValue(vals);
    }

    void close()
    {
        bs_times.append(0x0F, 4);
        bs_times.append(UINT32_MAX, 32);
        //padding
        bs_times.append(0, 64);
        bs_times.close();

        //padding
        bs_values.append(0, 64);
        bs_values.close();
    }

    void compressTimestamp(long timestamp)
    {
        // a) Calculate the delta of delta
        int64_t newDelta = (timestamp - storedTimestamp);
        int64_t deltaD = newDelta - storedDelta;

        if (deltaD == 0)
        {
            bs_times.push_back(0);
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
                bs_times.append(deltaD, 9);
                break;
            case 8:
            case 9:
                //DELTA_9_MASK adds '110' to deltaD
                deltaD |= DELTA_9_MASK;
                bs_times.append(deltaD, 12);
                break;
            case 10:
            case 11:
            case 12:
                //DELTA_12_MASK adds '1110' to deltaD
                deltaD |= DELTA_12_MASK;
                bs_times.append(deltaD, 16);
                break;
            default:
                // Append '1111'
                bs_times.append(0x0F, 4);
                bs_times.append(deltaD, 32);
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
            auto x = storedValues[i];
            auto y = values[i];
            uint64_t *a = (uint64_t *)&x;
            uint64_t *b = (uint64_t *)&y;
            uint64_t xor_ = *a ^ *b;

            if (xor_ == 0)
            {
                // Write 0
                bs_values.push_back(0);
                // countA++;
            }
            else
            {
                int leadingZeros = __builtin_clzll(xor_);
                int trailingZeros = __builtin_ctzll(xor_);

                if (leadingZeros >= 32)
                {
                    leadingZeros = 31;
                }

                if (leadingZeros == trailingZeros)
                {
                    xor_ = xor_ >> 1 << 1;
                    trailingZeros = 1;
                }

                // Store bit '1'
                bs_values.push_back(1);

                if (leadingZeros >= storedLeadingZeros[i] && trailingZeros >= storedTrailingZeros[i])
                {
                    bs_values.push_back(0);
                    int significantBits = 64 - storedLeadingZeros[i] - storedTrailingZeros[i];
                    xor_ >>= storedTrailingZeros[i];
                    bs_values.append(xor_, significantBits);
                    // countB++;
                    // auto bucket = (2 + significantBits) / 8;
                    // countB_vec[bucket]++;
                }
                else
                {
                    // out.push_back(1);
                    // out.append(leadingZeros, 5); // Number of leading zeros in the next 5 bits

                    int significantBits = 64 - leadingZeros - trailingZeros;

                    bs_values.append((((0x20 ^ leadingZeros) << 6) ^ (significantBits)), 12);
                    xor_ >>= trailingZeros;                  // Length of meaningful bits in the next 6 bits
                    bs_values.append(xor_, significantBits); // Store the meaningful bits of XOR

                    storedLeadingZeros[i] = leadingZeros;
                    storedTrailingZeros[i] = trailingZeros;
                    // countC++;
                    // auto bucket = (13 + significantBits) / 8;
                    // countC_vec[bucket]++;
                }
            }
            storedValues[i] = values[i];
        }
    }
};
