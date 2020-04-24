#include <vector>
#include <string>
#include "succinct/bit_vector.hpp"


inline int64_t decodeZZ(uint64_t i)
{
    return (i >> 1) ^ (-(i & 1));
}

struct PairMulti
{

    long timestamp;
    std::vector<double> value;

    PairMulti(uint64_t t, std::vector<double> const &v) : timestamp(t), value(v) {}

    std::string toString()
    {
        std::string tmp = std::to_string(timestamp);
        for (auto d : value)
        {
            tmp = tmp + " | " + std::to_string(d);
        }
        return tmp;
    }
};

struct DecompressorMulti
{

    std::vector<uint64_t> storedLeadingZeros;
    std::vector<uint64_t> storedTrailingZeros;
    std::vector<double> storedVal;
    uint8_t FIRST_DELTA_BITS = 14;

    long storedTimestamp = 0;
    long storedDelta = 0;

    long blockTimestamp = 0;

    bool endOfStream = false;

    succinct::bit_vector *in;
    size_t in_size = 0;
    size_t curr_pos = 0;

    int ncols;

    DecompressorMulti(succinct::bit_vector_builder *input, int n)
    {
        in = new succinct::bit_vector(input);
        in_size = in->size();
        ncols = n;

        storedLeadingZeros = std::vector<uint64_t>(ncols, 0);
        storedTrailingZeros = std::vector<uint64_t>(ncols, 0);
        storedVal = std::vector<double>(ncols, 0);

        readHeader();
    }

    void readHeader()
    {
        blockTimestamp = in->get_bits(0, 64);
        curr_pos += 64;
    }

    /**
     * Returns the next pair in the time series, if available.
     *
     * @return Pair if there's next value, null if series is done.
     */
    PairMulti readPair()
    {
        // if (endOfStream)
        // {
        //     return null;
        // }
        return PairMulti(storedTimestamp, storedVal);
    }

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
            storedDelta = in->get_bits(curr_pos, FIRST_DELTA_BITS);
            curr_pos += FIRST_DELTA_BITS;

            if (storedDelta == (1 << 14) - 1)
            {
                endOfStream = true;
                return;
            }
            for (int i = 0; i < ncols; i++)
            {
                auto read = in->get_bits(curr_pos, 64);
                double *p = (double *)&read;
                storedVal[i] = *p;
                curr_pos += 64;
            }
            storedTimestamp = blockTimestamp + storedDelta;
        }
        // else
        // {
        //     nextTimestamp();
        //     nextValue();
        // }
    }

    // int bitsToRead()
    // {
    //     int val = in.nextClearBit(4);
    //     int toRead = 0;

    //     switch (val)
    //     {
    //     case 0x00:
    //         break;
    //     case 0x02:
    //         toRead = 7; // '10'
    //         break;
    //     case 0x06:
    //         toRead = 9; // '110'
    //         break;
    //     case 0x0e:
    //         toRead = 12;
    //         break;
    //     case 0x0F:
    //         toRead = 32;
    //         break;
    //     }

    //     return toRead;
    // }

    // void nextTimestamp()
    // {
    //     // Next, read timestamp
    //     long deltaDelta = 0;
    //     int toRead = bitsToRead();
    //     if (toRead > 0)
    //     {
    //         deltaDelta = in.getLong(toRead);
    //         if (toRead == 32)
    //         {
    //             if ((int)deltaDelta == 0xFFFFFFFF)
    //             {
    //                 // End of stream
    //                 endOfStream = true;
    //                 return;
    //             }
    //         }
    //         deltaDelta = decodeZigZag32(deltaDelta);
    //         deltaDelta = (int)(deltaDelta);
    //     }

    //     storedDelta = storedDelta + deltaDelta;
    //     storedTimestamp = storedDelta + storedTimestamp;
    //     // nextValue();
    // }

    // void nextValue()
    // {
    //     for (int i = 0; i < ncols; i++)
    //     {
    //         // Read value
    //         if (in.readBit())
    //         {
    //             // else -> same value as before
    //             if (in.readBit())
    //             {
    //                 // New leading and trailing zeros
    //                 storedLeadingZeros.set(i, (int)in.getLong(5));

    //                 byte significantBits = (byte)in.getLong(6);
    //                 if (significantBits == 0)
    //                 {
    //                     significantBits = 64;
    //                 }
    //                 storedTrailingZeros.set(i, 64 - significantBits - storedLeadingZeros.get(i));
    //             }
    //             long value = in.getLong(64 - storedLeadingZeros.get(i) - storedTrailingZeros.get(i));
    //             value <<= storedTrailingZeros.get(i);
    //             value = storedVal.get(i) ^ value;
    //             storedVal.set(i, value);
    //         }
    //     }
    // }

    // static long decodeZigZag32(final long n)
    // {
    //     return (n >>> 1) ^ -(n & 1);
    // }
};
