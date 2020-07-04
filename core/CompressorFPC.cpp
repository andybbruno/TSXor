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
    bool first_append = true;
    uint8_t FIRST_DELTA_BITS = 32;

    // std::vector<uint64_t> storedLeadingZeros;
    // std::vector<uint64_t> storedTrailingZeros;
    // std::vector<double> storedValues;
    long storedTimestamp = 0;
    long storedDelta = 0;
    long blockTimestamp = 0;

    BitStream bstream;

    CompressorFPC(uint64_t timestamp)
    {
        blockTimestamp = timestamp;
        addHeader(timestamp);
    }

    void addHeader(uint64_t timestamp)
    {
        bstream.append(timestamp, 64);
    }

    void addValue(uint64_t timestamp, std::vector<double> const &vals)
    {
        if (first_append)
        {
            fpc = std::vector<FPC>(vals.size());
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
        bstream.append(storedDelta, FIRST_DELTA_BITS);

        for (int i = 0; i < values.size(); i++)
        {
            fpc[i].head(values[i]);
            uint64_t x = *((uint64_t *)&values[i]);
            bstream.append(x, 64);
        }
    }

    void close()
    {
        bstream.close();
    }

    void compressTimestamp(long timestamp)
    {
        // a) Calculate the delta of delta
        int64_t newDelta = (timestamp - storedTimestamp);
        int64_t deltaD = newDelta - storedDelta;

        if (deltaD == 0)
        {
            bstream.push_back(0);
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
                bstream.append(deltaD, 9);
                break;
            case 8:
            case 9:
                //DELTA_9_MASK adds '110' to deltaD
                deltaD |= DELTA_9_MASK;
                bstream.append(deltaD, 12);
                break;
            case 10:
            case 11:
            case 12:
                //DELTA_12_MASK adds '1110' to deltaD
                deltaD |= DELTA_12_MASK;
                bstream.append(deltaD, 16);
                break;
            default:
                // Append '1111'
                bstream.append(0x0F, 4);
                bstream.append(deltaD, 32);
                // bstream.append(deltaD, 64);
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
            bstream.append(code.first, 4);

            auto len = 64 - (8 * (code.first & (~((UINT64_MAX << 3)))));
            bstream.append(code.second, len);
        }
    }
};
