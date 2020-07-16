#include <vector>
#include <iostream>
#include <string>
#include "fpc_utils/fpc.cpp"
#include "../lib/Zigzag.hpp"
#include "../lib/BitStream.cpp"

struct DecompressorFPC
{
    std::vector<FPC> fpc;

    std::vector<double> storedVal;

    uint64_t storedTimestamp = 0;
    uint64_t storedDelta = 0;

    bool endOfStream = false;

    BitStream bs_times;
    BitStream bs_values;
    uint64_t ncols;

    DecompressorFPC(BitStream const &bs_ts, BitStream const &bs_val, uint64_t n)
    {
        bs_times = bs_ts;
        bs_values = bs_val;
        ncols = n;
        storedVal = std::vector<double>(ncols, 0);
        fpc = std::vector<FPC>(ncols);

        storedTimestamp = bs_times.get(64);

        for (int i = 0; i < ncols; i++)
        {
            uint64_t read = bs_values.get(64);
            double *p = (double *)&read;
            storedVal[i] = *p;
        }
    }

    bool hasNext()
    {
        nextTimestamp();
        if (endOfStream == false)
            nextValue();
        return !endOfStream;
    }

    uint64_t bitsToRead()
    {
        uint64_t val = bs_times.nextZeroWithin(4);
        uint64_t toRead = 0;

        switch (val)
        {
        case 0x00:
            break;
        case 0x02:
            toRead = 7; // '10'
            break;
        case 0x06:
            toRead = 9; // '110'
            break;
        case 0x0e:
            toRead = 12;
            break;
        case 0x0F:
            toRead = 32;
            // toRead = 64;
            break;
        }

        return toRead;
    }

    void nextTimestamp()
    {
        // Next, read timestamp
        uint64_t deltaDelta = 0;
        uint64_t toRead = bitsToRead();
        if (toRead > 0)
        {
            deltaDelta = bs_times.get(toRead);
            if (toRead == 32)
            {
                if (deltaDelta == UINT32_MAX)
                {
                    endOfStream = true;
                    return;
                }
            }
            deltaDelta = zz::decode(deltaDelta);
        }

        storedDelta = storedDelta + deltaDelta;
        storedTimestamp = storedDelta + storedTimestamp;
    }

    void nextValue()
    {
        for (int i = 0; i < ncols; i++)
        {
            bool use_fcm = bs_values.get(1);
            auto zeros = bs_values.get(3);
            auto body = bs_values.get(64 - (8 * zeros));

            double dec = fpc[i].decode(use_fcm, body);

            storedVal[i] = dec;
        }
    }
};
