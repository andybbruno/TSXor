#include <vector>
#include <map>
#include <string>
#include "succinct/bit_vector.hpp"
#include "lib/zigzag.hpp"
#include "lib/elias.hpp"

const auto &ENCODE = elias::gamma;

#define DELTA_7_MASK 0x02 << 7;
#define DELTA_9_MASK 0x06 << 9;
#define DELTA_12_MASK 0x0E << 12;

inline uint32_t count_decimal(uint64_t v)
{
    return 1 +
           (std::uint32_t)(v >= 10) +
           (std::uint32_t)(v >= 100) +
           (std::uint32_t)(v >= 1000) +
           (std::uint32_t)(v >= 10000) +
           (std::uint32_t)(v >= 100000) +
           (std::uint32_t)(v >= 1000000) +
           (std::uint32_t)(v >= 10000000) +
           (std::uint32_t)(v >= 100000000) +
           (std::uint32_t)(v >= 1000000000) +
           (std::uint32_t)(v >= 10000000000ull) +
           (std::uint32_t)(v >= 100000000000ull) +
           (std::uint32_t)(v >= 1000000000000ull) +
           (std::uint32_t)(v >= 10000000000000ull) +
           (std::uint32_t)(v >= 100000000000000ull) +
           (std::uint32_t)(v >= 1000000000000000ull) +
           (std::uint32_t)(v >= 10000000000000000ull) +
           (std::uint32_t)(v >= 100000000000000000ull) +
           (std::uint32_t)(v >= 1000000000000000000ull) +
           (std::uint32_t)(v >= 10000000000000000000ull);
}

struct CompressorMultiElias
{
    uint8_t FIRST_DELTA_BITS = 14;

    std::vector<uint64_t> storedLeadingZeros;
    std::vector<uint64_t> storedTrailingZeros;
    std::vector<double> storedValues;
    long storedTimestamp = 0;
    long storedDelta = 0;
    long blockTimestamp = 0;

    succinct::bit_vector_builder out;

    // We should have access to the series?

    CompressorMultiElias(uint64_t timestamp)
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

        // non funziona se storedDelta == 0
        // out.writeBits(storedDelta, FIRST_DELTA_BITS);
        for (double d : values)
        {
            uint64_t *x = (uint64_t *)&d;
            out.writeBits(*x, 64);
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
        out.skipBit();

        // map.insert(std::make_pair("count 0", count_0));
        // map.insert(std::make_pair("count A", count_A));
        // map.insert(std::make_pair("count B", count_B));
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

        // Add one in order to compute delta encoding
        // Remember to subtract one in DECODING!!
        long deltaD = newDelta - storedDelta + 1;

        deltaD = zz::encode(deltaD);
        auto delta = ENCODE(deltaD);

        // if ((delta.second > 63) || ((delta.first >> delta.second) == 0))
        // {
        //     std::cout << newDelta - storedDelta + 1 << std::endl;
        //     std::cout << delta.first << " " << delta.second << std::endl;
        // }

        out.append_bits(delta.first, delta.second);

        storedDelta = newDelta;
        storedTimestamp = timestamp;
    }

    void compressValue(std::vector<double> values)
    {
        for (int i = 0; i < values.size(); i++)
        {
            double diff = storedValues[i] - values[i];

            if (diff == 0)
            {
                // Write 0
                out.push_back(0);
            }
            else if (isnan(diff))
            {
                // Write '10'
                // out.push_back(1);
                // out.push_back(0);
                // out.append_bits(NAN, 63);
            }
            else
            {
                // Write '11'
                out.push_back(1);
                // out.push_back(1);

                uint64_t int_part = zz::encode((int64_t)diff);
                uint32_t dec_part = count_decimal(diff);

                auto d_int = ENCODE(int_part);
                auto d_dec = ENCODE(dec_part);

                // if ((d_int.second > 63) || ((d_int.first >> d_int.second) == 0))
                // {
                //     std::cout << diff << std::endl;
                //     std::cout << d_int.first << " " << d_int.second << std::endl;
                // }
                // if ((d_dec.second > 63) || ((d_dec.first >> d_dec.second) == 0))
                // {
                //     std::cout << diff << std::endl;
                //     std::cout << d_dec.first << " " << d_dec.second << std::endl;
                // }

                out.append_bits(d_int.first, d_int.second);
                out.append_bits(d_dec.first, d_dec.second);
            }
            storedValues[i] = values[i];
        }
    }
};
