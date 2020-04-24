#include <vector>
#include <map>
#include "lib/zigzag.hpp"
#include "succinct/bit_vector.hpp"


#define DELTA_7_MASK 0x02 << 7;
#define DELTA_9_MASK 0x06 << 9;
#define DELTA_12_MASK 0x0E << 12;


struct CompressorMulti
{
    uint8_t FIRST_DELTA_BITS = 14;

    std::vector<uint64_t> storedLeadingZeros;
    std::vector<uint64_t> storedTrailingZeros;
    std::vector<double> storedValues;
    long storedTimestamp = 0;
    long storedDelta = 0;
    long blockTimestamp = 0;

    succinct::bit_vector_builder out;

    std::map<std::string, int> mymap;

    // We should have access to the series?

    CompressorMulti(uint64_t timestamp)
    {
        blockTimestamp = timestamp;
        addHeader(timestamp);
    }

    void addHeader(uint64_t timestamp)
    {
        // One byte: length of the first delta
        // One byte: precision of timestamps
        out.writeBits(timestamp, 64);
    }

    /**
     * Adds a new uint64_t value to the series. Note, values must be inserted in order.
     *
     * @param timestamp Timestamp which is inside the allowed time block (default 24
     *                  hours with millisecond precision)
     * @param value     next floating point value in the series
     */

    void addValue(uint64_t timestamp, std::vector<double> const &vals)
    {
        if (storedTimestamp == 0)
        {
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
        storedValues = values;

        out.writeBits(storedDelta, FIRST_DELTA_BITS);
        for (double d : values)
        {
            out.writeBits(d, 64);
        }

        storedLeadingZeros = std::vector<uint64_t>(values.size(), 0);
        storedTrailingZeros = std::vector<uint64_t>(values.size(), 64);
    }

    /**
     * Closes the block and writes the remaining stuff to the BitOutput.
     */

    void close()
    {
        // These are selected to test interoperability and correctness of the solution,
        // this can be read with go-tsz
        // out.writeBits(0x0F, 4);
        // out.writeBits(0xFFFFFFFF, 32);
        // out.skipBit();
        // out.flush();
    }

    /**
     * Difference to the original Facebook paper, we store the first delta as 27
     * bits to allow millisecond accuracy for a one day block.
     *
     * Also, the timestamp delta-delta is not good for millisecond compressions..
     *
     * @param timestamp epoch
     */
    void compressTimestamp(long timestamp)
    {
        // a) Calculate the delta of delta
        long newDelta = (timestamp - storedTimestamp);
        long deltaD = newDelta - storedDelta;

        if (deltaD == 0)
        {
            out.push_back(0);
        }
        else
        {
            deltaD = zz::encode(deltaD);
            auto length = 32 - __builtin_clz(deltaD);

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
                out.append_bits(deltaD, 9);
                break;
            case 8:
            case 9:
                //DELTA_9_MASK adds '110' to deltaD
                deltaD |= DELTA_9_MASK;
                out.append_bits(deltaD, 12);
                break;
            case 10:
            case 11:
            case 12:
                //DELTA_12_MASK adds '1110' to deltaD
                deltaD |= DELTA_12_MASK;
                out.append_bits(deltaD, 16);
                break;
            default:
                // Append '1111'
                out.append_bits(0x0F, 4);
                out.append_bits(deltaD, 32);
                break;
            }
        }

        storedDelta = newDelta;
        storedTimestamp = timestamp;
    }

    void compressValue(std::vector<double> values)
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
                out.skipBit();
            }
            else
            {
                int leadingZeros = __builtin_clzll(xor_);
                int trailingZeros = __builtin_ctzll(xor_);

                
                // uint64_t value = xor_ >> trailingZeros;
                std::string key = std::to_string((uint64_t)y);

                if (mymap.find(key) == mymap.end())
                {
                    mymap[key]++;
                }
                else
                {
                    mymap.insert(std::make_pair(key, 1));
                }

                // Check overflow of leading? Can't be 32!
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
                out.writeBit();

                if (leadingZeros >= storedLeadingZeros[i] && trailingZeros >= storedTrailingZeros[i])
                {
                    out.skipBit();
                    int significantBits = 64 - storedLeadingZeros[i] - storedTrailingZeros[i];
                    xor_ >>= storedTrailingZeros[i];
                    out.writeBits(xor_, significantBits);
                }
                else
                {
                    out.writeBit();
                    out.writeBits(leadingZeros, 5); // Number of leading zeros in the next 5 bits

                    int significantBits = 64 - leadingZeros - trailingZeros;
                    out.writeBits(significantBits, 6);
                    xor_ >>= trailingZeros;               // Length of meaningful bits in the next 6 bits
                    out.writeBits(xor_, significantBits); // Store the meaningful bits of XOR

                    storedLeadingZeros[i] = leadingZeros;
                    storedTrailingZeros[i] = trailingZeros;
                }
            }
            storedValues[i] = values[i];
        }
    }
};
