#include "BitStream.cpp"
#include "DataPoint.cpp"
#include <string>
#include <fstream>
#include <iostream>
struct Compressor
{
    //Both data and address are const
    DataPoint *header;
    std::fstream outfile;

    bool FIRST_BLOCK = true;

    DataPoint *sec_last;
    DataPoint *last;

    BitStream bs;

    Compressor(DataPoint *head, std::string filename) : header(head)
    {
        bs.addValue(head->timestamp, 64, "Compressor constructor timestamp");

        for (auto val : head->values)
            bs.addValue(val, 64, "Compressor constructor value");
    };

    void deltaEncoding(int64_t delta)
    {
        if (delta == 0)
        {
            bs.addBit(0);
        }
        else if ((-63 < delta) && (delta < 64))
        {
            delta += 63;
            bs.addBit(1);
            bs.addBit(0);
            bs.addValue(delta, 7, "deltaEncoding 7");
        }
        else if ((-255 < delta) && (delta < 256))
        {
            delta += 255;
            bs.addBit(1);
            bs.addBit(1);
            bs.addBit(0);
            bs.addValue(delta, 9, "deltaEncoding 9");
        }
        else if ((-2047 < delta) && (delta < 2048))
        {
            delta += 2047;
            bs.addBit(1);
            bs.addBit(1);
            bs.addBit(1);
            bs.addBit(0);
            bs.addValue(delta, 12, "deltaEncoding 12");
        }
        else
        {
            if (delta < 0){
                delta ^= 0xFFFFFFFF00000000;
            }
            
            bs.addBit(1);
            bs.addBit(1);
            bs.addBit(1);
            bs.addBit(1);

            bs.addValue(delta, 32, "deltaEncoding 32");
        }
    }

    void valuesEncoding(DataPoint *curr, DataPoint *prev)
    {
        for (int i = 0; i < curr->values.size(); i++)
        {
            auto current = (uint64_t *)&curr->values[i];
            auto past = (uint64_t *)&prev->values[i];
            uint64_t xored = *current ^ *past;
            curr->xorWithPrev[i] = xored;

            if (xored == 0)
            {
                bs.addBit(0);
            }
            else
            {
                // count leading and traling zeros
                uint64_t leadingZeros = __builtin_clzll(xored);
                uint64_t trailingZeros = __builtin_ctzll(xored);

                uint64_t prevXorLeadingZeros = __builtin_clzll(prev->xorWithPrev[i]);
                uint64_t prevXorTrailingZeros = __builtin_ctzll(prev->xorWithPrev[i]);

                uint64_t corePart = xored >> trailingZeros;
                int coreLength = 64 - leadingZeros - trailingZeros;

                if ((leadingZeros == prevXorLeadingZeros) && (trailingZeros == prevXorTrailingZeros))
                {
                    // '1' + control bit '0'
                    bs.addBit(1);
                    bs.addBit(0);
                }
                else
                {
                    // '1' + control bit '1'
                    bs.addBit(1);
                    bs.addBit(1);

                    //  5 bits for leading zeros
                    bs.addValue(leadingZeros, 5, "deltaValue 5 bit");

                    //  6 bits for the length of the core part of the XOR
                    bs.addValue(coreLength, 6, "deltaValue 6 bit");
                }

                bs.addValue(corePart, coreLength, "deltaValue finale");
            }
        }
    }

    void append(DataPoint *dp)
    {
        auto values = dp->values;
        auto ts = dp->timestamp;

        if (FIRST_BLOCK)
        {
            auto delta = ts - header->timestamp;
            bs.addValue(delta, 14, "first block");
            valuesEncoding(dp, header);

            sec_last = header;
            last = dp;
            FIRST_BLOCK = false;
        }
        else
        {
            int64_t delta = (ts - last->timestamp) - (last->timestamp - sec_last->timestamp);

            deltaEncoding(delta);
            valuesEncoding(dp, last);

            sec_last = last;
            last = dp;
        }
    }
};