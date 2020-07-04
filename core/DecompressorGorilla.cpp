#include <vector>
#include <iostream>
#include <string>
#include "../lib/BitStream.cpp"
#include "../lib/Zigzag.hpp"

// struct PairMulti
// {
//     long timestamp;
//     std::vector<double> value;

//     PairMulti(uint64_t t, std::vector<double> const &v) : timestamp(t), value(v) {}

//     std::string toString()
//     {
//         std::string tmp = std::to_string(timestamp);
//         for (auto d : value)
//         {
//             tmp = tmp + " | " + std::to_string(d);
//         }
//         return tmp;
//     }
// };

struct DecompressorMulti
{

    std::vector<uint64_t> storedLeadingZeros;
    std::vector<uint64_t> storedTrailingZeros;
    std::vector<double> storedVal;
    uint8_t FIRST_DELTA_BITS = 32;

    uint64_t storedTimestamp = 0;
    uint64_t storedDelta = 0;
    uint64_t blockTimestamp = 0;

    bool endOfStream = false;

    // BitVector in;
    BitStream in;
    uint64_t ncols;

    DecompressorMulti(BitStream const &input, uint64_t n)
    {
        in = input;
        ncols = n;

        storedLeadingZeros = std::vector<uint64_t>(ncols, 0);
        storedTrailingZeros = std::vector<uint64_t>(ncols, 0);
        storedVal = std::vector<double>(ncols, 0);

        readHeader();
    }

    void readHeader()
    {
        blockTimestamp = in.get(64);
    }

    /**
     * Returns the next pair in the time series, if available.
     *
     * @return Pair if there's next value, null if series is done.
     */
    // PairMulti readPair()
    // {
    //     // if (endOfStream)
    //     // {
    //     //     return null;
    //     // }
    //     return PairMulti(storedTimestamp, storedVal);
    // }

    bool hasNext()
    {
        next();
        return !endOfStream;
    }

    void next()
    {
        if (storedTimestamp == 0)
        {
            // First item to read
            storedDelta = in.get(FIRST_DELTA_BITS);

            if (storedDelta == 0xFFFFFFFF)
            {
                endOfStream = true;
                return;
            }
            for (int i = 0; i < ncols; i++)
            {
                uint64_t read = in.get(64);
                double *p = (double *)&read;
                storedVal[i] = *p;
            }
            storedTimestamp = blockTimestamp + storedDelta;
        }
        else
        {
            nextTimestamp();
            // nextValue();
        }
    }

    uint64_t bitsToRead()
    {
        uint64_t val = in.nextZeroWithin(4);
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
            deltaDelta = in.get(toRead);
            if (toRead == 32)
            // if (toRead == 64)
            {
                if (deltaDelta == UINT32_MAX)
                // if (deltaDelta == UINT64_MAX)
                {
                    // End of stream
                    endOfStream = true;
                    return;
                }
            }
            deltaDelta = zz::decode(deltaDelta);
            // deltaDelta = (int)(deltaDelta);
        }

        storedDelta = storedDelta + deltaDelta;
        storedTimestamp = storedDelta + storedTimestamp;
        nextValue();
    }

    void nextValue()
    {
        for (int i = 0; i < ncols; i++)
        {
            // Read value
            // If 1 means that the value has not changed, hence no ops perfomed
            if (in.readBit())
            {
                // else -> same value as before
                if (in.readBit())
                {
                    // New leading and trailing zeros
                    storedLeadingZeros[i] = in.get(5);

                    uint64_t significantBits = in.get(6);
                    if (significantBits == 0)
                    {
                        significantBits = 64;
                    }
                    storedTrailingZeros[i] = 64 - significantBits - storedLeadingZeros[i];
                }
                uint64_t value = in.get(64 - storedLeadingZeros[i] - storedTrailingZeros[i]);
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
