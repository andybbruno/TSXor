#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include "zigzag.hpp"
#include "BitStream.cpp"

struct DecompressorXorCache
{
    std::vector<Cache> cache;

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

    std::vector<uint8_t> bytes;
    uint64_t current_idx = 0;

    DecompressorXorCache(BitStream &input, std::vector<uint8_t> &bts, uint64_t n)
    {
        in = input;
        ncols = n;
        bytes = bts;
        storedVal = std::vector<double>(ncols, 0);
        cache = std::vector<Cache>(ncols);
        readHeader();
    }

    void readHeader()
    {
        blockTimestamp = in.get(64);
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
            storedDelta = in.get(FIRST_DELTA_BITS);

            if (storedDelta == (1 << 14) - 1)
            {
                endOfStream = true;
                return;
            }
            for (int i = 0; i < ncols; i++)
            {
                uint64_t read = readBytes(8);
                cache[i].insert(read);
                double p = (*(double *)&read);
                storedVal[i] = p;
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
        uint64_t final_val;
        uint64_t offset;
        uint64_t info;
        uint64_t trail_zeros_bytes;
        uint64_t xor_bytes;
        uint64_t xor_;
        uint64_t head;
        for (int i = 0; i < ncols; i++)
        {
            head = readBytes(1);

            if (head < 128)
            {
                final_val = cache[i].get(head);
            }
            else if (head == 255)
            {
                final_val = readBytes(8);
            }
            else
            {
                offset = head & (~((UINT64_MAX << 7)));
                info = readBytes(1);
                trail_zeros_bytes = info >> 4;
                xor_bytes = info & (~((UINT64_MAX << 4)));
                xor_ = readBytes(xor_bytes) << (8 * trail_zeros_bytes);
                final_val = xor_ ^ cache[i].get(offset);
            }
            cache[i].insert(final_val);
            double p = (*(double *)&final_val);
            storedVal[i] = p;
        }
        // for (int i = 0; i < ncols; i++)
        // {
        //     cache[i].insert(final_val[i]);
        //     double p = (*(double *)&(final_val[i]));
        //     storedVal[i] = p;
        // }
    }

    inline uint64_t readBytes(size_t len)
    {
        uint64_t val = 0;
        for (int i = 0; i < len; i++)
        {
            val |= bytes[current_idx];
            current_idx++;
            if (i != (len - 1))
            {
                val <<= 8;
            }
        }
        return val;
    }
};
