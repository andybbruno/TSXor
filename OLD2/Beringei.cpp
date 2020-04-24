#include "BitStream.cpp"
#include "succinct/bit_vector.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include "lib/zigzag.hpp"

#define DELTA_7_MASK 0x02 << 7;
#define DELTA_9_MASK 0x06 << 9;
#define DELTA_12_MASK 0x0E << 12;

struct Beringei
{
    std::fstream outfile;

    bool FIRST_BLOCK = true;

    uint64_t storedLeadingZeros;
    uint64_t storedTrailingZeros;

    double last_val;

    uint64_t last_time;
    uint64_t sec_last_time;

    uint64_t head_time;
    uint64_t head_val;

    succinct::bit_vector_builder bs;

    Beringei(uint64_t time, uint64_t val) : head_time(time), head_val(val)
    {
        last_val = 0;
        storedLeadingZeros = 64;
        storedTrailingZeros = 0;

        bs.append_bits(time, 64, "Beringei TIME");
        bs.append_bits(val, 64, "Beringei VAL");
        
        last_val = val;
        last_time = time;
        sec_last_time = 0;
    };

    //cast to 32 bits!!
    void deltaEncoding(int32_t delta)
    {
        if (delta == 0)
        {
            bs.push_back(0);
        }
        else
        {
            delta = zz::encode(delta);
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
                break;
            case 8:
            case 9:
                //DELTA_9_MASK adds '110' to delta
                delta |= DELTA_9_MASK;
                bs.append_bits(delta, 12, "Delta 12");
                break;
            case 10:
            case 11:
            case 12:
                //DELTA_12_MASK adds '1110' to delta
                delta |= DELTA_12_MASK;
                bs.append_bits(delta, 16, "Delta 16");
                break;
            default:
                // Append '1111'
                bs.append_bits(0x0F, 4, "Delta mask 4");
                bs.append_bits(delta, 32, "Delta 32");
                break;
            }
        }
    }

    void writeExistingLeading(uint64_t xored_)
    {
        // Control bit '0'
        bs.push_back(0);
        xored_ >>= storedTrailingZeros;
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

        storedLeadingZeros = leadingZeros;
        storedTrailingZeros = trailingZeros;
    }

    void valuesEncoding(double actual)
    {
        auto a = (uint64_t *)&actual;
        auto b = (uint64_t *)&last_val;
        uint64_t xored = *a ^ *b;
        
        // std::cout << std::hex << xored << std::endl;
        
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
            if ((leadingZeros >= storedLeadingZeros) && (trailingZeros >= storedTrailingZeros))
            {
                writeExistingLeading(xored);
            }
            else
            {
                writeNewLeading(xored, leadingZeros, trailingZeros);
            }
        }
    }

    void append(uint64_t ts, double val)
    {
        if (FIRST_BLOCK)
        {
            auto delta = ts - head_time;
            bs.append_bits(delta, 14, "First Block");

            sec_last_time = head_time;
            last_time = ts;

            valuesEncoding(val);
            last_val = val;

            FIRST_BLOCK = false;
        }
        else
        {
            int64_t delta = (ts - last_time) - (last_time - sec_last_time);
            deltaEncoding(delta);
            sec_last_time = last_time;
            last_time = ts;

            valuesEncoding(val);
            last_val = val;
        }
    }
};