#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include "../lib/Zigzag.hpp"
#include "../lib/BitStream.cpp"
#include "../lib/Window.cpp"

struct DecompressorLZXOR
{
    std::vector<double> storedVal;

    uint64_t storedTimestamp = 0;
    uint64_t storedDelta = 0;
    uint64_t current_idx = 0;
    
    bool endOfStream = false;

    BitStream bs_times;
    std::vector<uint8_t> bt_values;
    std::vector<Window> window;

    uint64_t ncols;

    DecompressorLZXOR(BitStream &bs_ts, std::vector<uint8_t> &bt_val, uint64_t n)
    {
        bs_times = bs_ts;
        bt_values = bt_val;
        ncols = n;
        storedVal = std::vector<double>(ncols, 0);
        window = std::vector<Window>(ncols);

        storedTimestamp = bs_times.get(64);

        for (int i = 0; i < ncols; i++)
        {
            uint64_t read = readBytes(8);
            window[i].insert(read);
            double p = (*(double *)&read);
            storedVal[i] = p;
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
            // deltaDelta = (int)(deltaDelta);
        }

        storedDelta = storedDelta + deltaDelta;
        storedTimestamp = storedDelta + storedTimestamp;
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
                final_val = window[i].get(head);
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
                final_val = xor_ ^ window[i].get(offset);
            }
            window[i].insert(final_val);
            double p = (*(double *)&final_val);
            storedVal[i] = p;
        }
    }

    uint64_t readBytes(size_t len)
    {
        uint64_t val = 0;
        for (int i = 0; i < len; i++)
        {
            val |= bt_values[current_idx];
            current_idx++;
            if (i != (len - 1))
            {
                val <<= 8;
            }
        }
        return val;
    }
};
