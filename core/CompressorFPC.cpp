#include <vector>
#include <map>
#include <string>
#include "fpc_utils/fpc.cpp"
#include "../lib/BitStream.cpp"
#include "../lib/Zigzag.hpp"

#define DELTA_7_MASK 0x02 << 7;
#define DELTA_9_MASK 0x06 << 9;
#define DELTA_12_MASK 0x0E << 12;

struct CompressorFPC
{
    std::vector<FPC> fpc;

    std::vector<double> storedValues;
    long storedTimestamp = 0;
    long storedDelta = 0;

    BitStream bs_times;
    BitStream bs_values;

    CompressorFPC(uint64_t timestamp, std::vector<double> const &values)
    {
        fpc = std::vector<FPC>(values.size());

        bs_times.append(timestamp, 64);

        for (double d : values)
        {
            uint64_t *x = (uint64_t *)&d;
            bs_values.append(*x, 64);
        }

        storedTimestamp = timestamp;
        storedValues = values;
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
            auto code = fpc[i].encode(values[i]);
            bs_values.append(code.first, 4);

            auto len = 64 - (8 * (code.first & (~((UINT64_MAX << 3)))));
            bs_values.append(code.second, len);
        }
    }
};
