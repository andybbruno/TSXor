#include <vector>
#include <map>
#include <string>
#include "bitset.cpp"

#define DELTA_7_MASK 0x02 << 7;
#define DELTA_9_MASK 0x06 << 9;
#define DELTA_12_MASK 0x0E << 12;


inline uint64_t encodeZZ(int64_t i)
{
    return (i >> 63) ^ (i << 1);
}

inline int64_t decodeZZ(uint64_t i){
    return (i >> 1) ^ (-(i & 1));
}


inline uint32_t digits(uint64_t v)
{
    return 1 + (std::uint32_t)(v >= 10) + (std::uint32_t)(v >= 100) + (std::uint32_t)(v >= 1000) + (std::uint32_t)(v >= 10000) + (std::uint32_t)(v >= 100000) + (std::uint32_t)(v >= 1000000) + (std::uint32_t)(v >= 10000000) + (std::uint32_t)(v >= 100000000) + (std::uint32_t)(v >= 1000000000) + (std::uint32_t)(v >= 10000000000ull) + (std::uint32_t)(v >= 100000000000ull) + (std::uint32_t)(v >= 1000000000000ull) + (std::uint32_t)(v >= 10000000000000ull) + (std::uint32_t)(v >= 100000000000000ull) + (std::uint32_t)(v >= 1000000000000000ull) + (std::uint32_t)(v >= 10000000000000000ull) + (std::uint32_t)(v >= 100000000000000000ull) + (std::uint32_t)(v >= 1000000000000000000ull) + (std::uint32_t)(v >= 10000000000000000000ull);
}

struct CompressorMulti
{
    std::map<std::string, int> map;
    int count_0 = 0;
    int count_A = 0;
    int count_B = 0;
    uint8_t FIRST_DELTA_BITS = 14;

    std::vector<uint64_t> storedLeadingZeros;
    std::vector<uint64_t> storedTrailingZeros;
    std::vector<double> storedValues;
    long storedTimestamp = 0;
    long storedDelta = 0;
    long blockTimestamp = 0;

    bitset out;

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
        out.writeBits(0x0F, 4);
        out.writeBits(0xFFFFFFFF, 32);
        out.push_back(0);

        map.insert(std::make_pair("count 0", count_0));
        map.insert(std::make_pair("count A", count_A));
        map.insert(std::make_pair("count B", count_B));
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
            deltaD = encodeZZ(deltaD);
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
                out.writeBits(deltaD, 9);
                break;
            case 8:
            case 9:
                //DELTA_9_MASK adds '110' to deltaD
                deltaD |= DELTA_9_MASK;
                out.writeBits(deltaD, 12);
                break;
            case 10:
            case 11:
            case 12:
                //DELTA_12_MASK adds '1110' to deltaD
                deltaD |= DELTA_12_MASK;
                out.writeBits(deltaD, 16);
                break;
            default:
                // Append '1111'
                out.writeBits(0x0F, 4);
                out.writeBits(deltaD, 32);
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
                out.push_back(0);

                count_0++;
            }
            else
            {
                int leadingZeros = __builtin_clzll(xor_);
                int trailingZeros = __builtin_ctzll(xor_);

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
                out.push_back(1);

                if (leadingZeros >= storedLeadingZeros[i] && trailingZeros >= storedTrailingZeros[i])
                {
                    out.push_back(0);
                    int significantBits = 64 - storedLeadingZeros[i] - storedTrailingZeros[i];
                    xor_ >>= storedTrailingZeros[i];
                    out.writeBits(xor_, significantBits);

                    auto dy = digits(y);
                    std::string s1 = dy < 10 ? "0" + std::to_string(dy) : std::to_string(dy);
                    std::string s2 = significantBits < 10 ? "0" + std::to_string(significantBits) : std::to_string(significantBits);
                    auto key = "A: " + s1 + " - " + s2;

                    if (map.find(key) != map.end())
                    {
                        map[key] = map[key] + 1;
                    }
                    else
                    {
                        map.insert(std::make_pair(key, 1));
                    }
                    count_A++;
                }
                else
                {
                    out.push_back(1);
                    out.writeBits(leadingZeros, 5); // Number of leading zeros in the next 5 bits

                    int significantBits = 64 - leadingZeros - trailingZeros;
                    out.writeBits(significantBits, 6);
                    xor_ >>= trailingZeros;               // Length of meaningful bits in the next 6 bits
                    out.writeBits(xor_, significantBits); // Store the meaningful bits of XOR

                    storedLeadingZeros[i] = leadingZeros;
                    storedTrailingZeros[i] = trailingZeros;

                    auto dy = digits(y);
                    std::string s1 = dy < 10 ? "0" + std::to_string(dy) : std::to_string(dy);
                    std::string s2 = significantBits < 10 ? "0" + std::to_string(significantBits) : std::to_string(significantBits);
                    auto key = "B: " + s1 + " - " + s2;
                    if (map.find(key) != map.end())
                    {
                        map[key] = map[key] + 1;
                    }
                    else
                    {
                        map.insert(std::make_pair(key, 1));
                    }
                    count_B++;
                }
            }
            storedValues[i] = values[i];
        }
    }
};
