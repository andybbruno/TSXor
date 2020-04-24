#include "BitStream.cpp"
#include "succinct/bit_vector.hpp"
#include <string>
#include <fstream>
#include <iostream>

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

struct Compressor
{
    std::fstream outfile;

    bool FIRST_BLOCK = true;

    std::vector<double> storedLeadingZeros;
    std::vector<double> storedTrailingZeros;

    std::vector<double> last_vals;

    uint64_t last_time;
    uint64_t sec_last_time;

    uint64_t head_time;
    std::vector<double> head_vals;

    succinct::bit_vector_builder bs;

    uint64_t ts_bits;

    Compressor(uint64_t time, std::vector<double> const &vals) : head_time(time), head_vals(vals)
    {
        ts_bits = 0;

        last_vals = std::vector<double>(vals.size(), 0);
        storedLeadingZeros = std::vector<double>(vals.size(), 64);
        storedTrailingZeros = std::vector<double>(vals.size(), 0);

        bs.append_bits(time, 64, "Beringei TIME");

        last_time = time;
        sec_last_time = 0;
    };

    //cast to 32 bits!!
    void deltaEncoding(int32_t delta)
    {
        if (delta == 0)
        {
            bs.push_back(0);
            ++ts_bits;
        }
        else
        {
            delta = encodeZZ(delta);
            auto length = 32 - __builtin_clz(delta);

            switch (length)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                //DELTA_7_MASK adds '10' to delta
                delta |= DELTA_7_MASK;
                bs.append_bits(delta, 9, "Delta 9");
                ts_bits += 9;
                break;
            case 8:
            case 9:
                //DELTA_9_MASK adds '110' to delta
                delta |= DELTA_9_MASK;
                bs.append_bits(delta, 12, "Delta 12");
                ts_bits += 12;
                break;
            case 10:
            case 11:
            case 12:
                //DELTA_12_MASK adds '1110' to delta
                delta |= DELTA_12_MASK;
                bs.append_bits(delta, 16, "Delta 16");
                ts_bits += 16;
                break;
            default:
                // Append '1111'
                bs.append_bits(0x0F, 4, "Delta mask 4");
                bs.append_bits(delta, 32, "Delta 32");
                ts_bits += 36;
                break;
            }
        }
    }

    void writeExistingLeading(uint64_t xored_, uint64_t trailZeros)
    {
        // Control bit '0'
        bs.push_back(0);
        xored_ >>= trailZeros;
        uint64_t nbits = 64 - __builtin_clzll(xored_);
        bs.append_bits(xored_, nbits, "writeExistingLeading");
    }
    void writeNewLeading(uint64_t xored_, uint64_t leadingZeros, uint64_t trailingZeros)
    {
        // Control bit '1'
        bs.push_back(1);

        //  5 bits for leading zeros
        bs.append_bits(leadingZeros, 5, "writeNewLeading 5");

        //  6 bits for significant bits length
        uint32_t significantBits = 64 - leadingZeros - trailingZeros;
        bs.append_bits(significantBits, 6, "writeNewLeading 6");

        xored_ >>= trailingZeros;
        bs.append_bits(xored_, significantBits, "writeNewLeading Final");
    }

    void valuesEncoding(std::vector<double> const &values)
    {
        for (int i = 0; i < values.size(); i++)
        {
            auto x = values[i];
            auto y = last_vals[i];
            uint64_t *a = (uint64_t *)&x;
            uint64_t *b = (uint64_t *)&y;
            uint64_t xored = *a ^ *b;

            if (xored == 0)
            {
                // Write 0
                bs.push_back(0);
            }
            else
            {
                // count leading and traling zeros
                uint64_t leadingZeros = __builtin_clzll(xored);
                uint64_t trailingZeros = __builtin_ctzll(xored);

                //Write 1
                bs.push_back(1);
                if ((leadingZeros >= storedLeadingZeros[i]) && (trailingZeros >= storedTrailingZeros[i]))
                {
                    writeExistingLeading(xored, storedTrailingZeros[i]);
                }
                else
                {
                    writeNewLeading(xored, leadingZeros, trailingZeros);
                    storedLeadingZeros[i] = leadingZeros;
                    storedTrailingZeros[i] = trailingZeros;
                }
            }
        }
    }

    void append(uint64_t ts, std::vector<double> const &values)
    {
        if (FIRST_BLOCK)
        {
            auto delta = ts - head_time;
            bs.append_bits(delta, 14, "First Block");
            ts_bits += 14;

            for (auto v : values)
                bs.append_bits(v, 64, "Beringei VAL");
            last_vals = values;

            sec_last_time = head_time;
            last_time = ts;

            FIRST_BLOCK = false;
        }
        else
        {
            int64_t delta = (ts - last_time) - (last_time - sec_last_time);
            deltaEncoding(delta);
            sec_last_time = last_time;
            last_time = ts;
        }

        valuesEncoding(values);
        last_vals = values;
    }
};