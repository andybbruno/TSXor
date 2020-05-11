#include <vector>
#include <map>
#include <string>
#include "BitStream.cpp"
#include "zigzag.hpp"
#include "HDP/hdp.cpp"

#define DELTA_7_MASK 0x02 << 7;
#define DELTA_9_MASK 0x06 << 9;
#define DELTA_12_MASK 0x0E << 12;

struct CompressorHDP
{
    HDP hdp;
    bool first_append = true;
    uint8_t FIRST_DELTA_BITS = 32;

    // std::vector<uint64_t> storedLeadingZeros;
    // std::vector<uint64_t> storedTrailingZeros;
    // std::vector<double> storedValues;
    long storedTimestamp = 0;
    long storedDelta = 0;
    long blockTimestamp = 0;

    BitStream out;

    CompressorHDP(uint64_t timestamp)
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
        if (first_append)
        {
            // hdp = std::vector<HDP>(vals.size());
            first_append = false;
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
            uint64_t x = *((uint64_t *)&values[i]);
            out.append(x, 64);
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
            auto code = hdp.encode(values[i]);
            // std::cout << "A: " << code.first << "\t\t\t";
            out.append(code.first, 8);

            if (code.first > 0)
            {
                auto lead_zeros = 8 * (code.first >> 4);
                auto trail_zeros = 8 * (code.first & (~((UINT64_MAX << 4))));

                auto len = 64 - lead_zeros - trail_zeros;
                code.second >>= trail_zeros;
                // std::cout << "B: " << code.second << " : " << len << std::endl;
                out.append(code.second, len);
            }
            // else
            // {
            //     std::cout << std::endl;
            // }
        }
    }
};
