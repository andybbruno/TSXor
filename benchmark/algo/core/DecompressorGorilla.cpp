#include <vector>
#include <iostream>
#include <string>
#include "lib/BitStream.cpp"
#include "lib/Zigzag.hpp"

struct DecompressorGorilla
{
    std::vector<uint64_t> storedLeadingZeros;
    std::vector<uint64_t> storedTrailingZeros;
    std::vector<double> storedVal;

    uint64_t storedTimestamp = 0;
    uint64_t storedDelta = 0;

    bool endOfStream = false;

    BitStream bs_times;
    BitStream bs_values;
    uint64_t ncols;

    DecompressorGorilla(BitStream const &bs_ts, BitStream const &bs_val, uint64_t n)
    {
        bs_times = bs_ts;
        bs_values = bs_val;
        ncols = n;
        storedLeadingZeros = std::vector<uint64_t>(ncols, 0);
        storedTrailingZeros = std::vector<uint64_t>(ncols, 0);
        storedVal = std::vector<double>(ncols, 0);

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
            // Read value
            // If 1 means that the value has not changed, hence no ops perfomed
            if (bs_values.readBit())
            {
                // else -> same value as before
                if (bs_values.readBit())
                {
                    // New leading and trailing zeros
                    storedLeadingZeros[i] = bs_values.get(5);

                    uint64_t significantBits = bs_values.get(6);
                    if (significantBits == 0)
                    {
                        significantBits = 64;
                    }
                    storedTrailingZeros[i] = 64 - significantBits - storedLeadingZeros[i];
                }
                uint64_t value = bs_values.get(64 - storedLeadingZeros[i] - storedTrailingZeros[i]);
                value <<= storedTrailingZeros[i];

                uint64_t *a = (uint64_t *)&storedVal[i];
                uint64_t *b = (uint64_t *)&value;
                uint64_t xor_ = *a ^ *b;
                double *p = (double *)&xor_;
                storedVal[i] = *p;
            }
        }
    }
};
