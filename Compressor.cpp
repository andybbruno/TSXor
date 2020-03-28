#include "DataPoint.cpp"
#include <string>
#include <fstream>
#include <iostream>
#include <boost/dynamic_bitset.hpp>

class Compressor
{
    //Both data and address are const
    DataPoint *header;
    std::fstream outfile;

    bool HEADER = false;
    bool FIRST_BLOCK = true;

    DataPoint *sec_last;
    DataPoint *last;
    // DataPoint *curr;

public:
    Compressor(DataPoint *head, std::string filename) : header(head)
    {
        outfile = std::fstream(filename, std::ios::out | std::ios::binary);
        if (!outfile)
        {
            std::cerr << "Unable to open file" << std::endl;
            exit(1); // terminate with error
        }

        //Try to write the header on the file
        try
        {
            outfile.write((char *)&header, sizeof(DataPoint));
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
        HEADER = true;
    };

    boost::dynamic_bitset<> deltaToBits(int64_t delta)
    {
        boost::dynamic_bitset<> s;

        if (delta == 0)
        {
            s = boost::dynamic_bitset<>(1, 0);
        }
        else if ((-63 < delta) && (delta < 64))
        {
            s = boost::dynamic_bitset<>(7, delta);
            s.push_back(false);
            s.push_back(true);
        }
        else if ((-255 < delta) && (delta < 256))
        {
            s = boost::dynamic_bitset<>(9, delta);
            s.push_back(false);
            s.push_back(true);
            s.push_back(true);
        }
        else if ((-2047 < delta) && (delta < 2048))
        {
            s = boost::dynamic_bitset<>(12, delta);
            s.push_back(false);
            s.push_back(true);
            s.push_back(true);
            s.push_back(true);
        }
        else
        {
            s = boost::dynamic_bitset<>(32, delta);
            s.push_back(true);
            s.push_back(true);
            s.push_back(true);
            s.push_back(true);
        }
        return s;
    }

    boost::dynamic_bitset<> concatDynBitSet(const boost::dynamic_bitset<> &bs1, const boost::dynamic_bitset<> &bs2)
    {
        boost::dynamic_bitset<> res = bs2;

        for (int i = 0; i < bs1.size(); i++)
        {
            bs1[i] == true ? res.push_back(true) : res.push_back(false);
        }
        return res;
    }

    std::vector<boost::dynamic_bitset<>> valuesToBits(DataPoint *curr, DataPoint *prev)
    {
        std::vector<boost::dynamic_bitset<>> result;

        for (int i = 0; i < curr->values.size(); i++)
        {
            boost::dynamic_bitset<> res;

            auto current = (uint64_t *)&curr->values[i];
            auto past = (uint64_t *)&prev->values[i];
            uint64_t xored = *current ^ *past;
            curr->xorWithPrev[i] = xored;

            // std::cout << std::bitset<64>(xored).to_string() << std::endl;

            if (xored == 0)
            {
                res = boost::dynamic_bitset<>(1, 0);
            }
            else
            {
                res.push_back(true);

                // conta leading e traling zero
                uint64_t leadingZeros = __builtin_clzll(xored);
                uint64_t trailingZeros = __builtin_ctzll(xored);

                uint64_t prevXorLeadingZeros = __builtin_clzll(prev->xorWithPrev[i]);
                uint64_t prevXorTrailingZeros = __builtin_ctzll(prev->xorWithPrev[i]);

                uint64_t corePart = xored >> trailingZeros;
                int coreLength = 64 - leadingZeros;

                if ((leadingZeros == prevXorLeadingZeros) && (trailingZeros == prevXorTrailingZeros))
                {
                    // control bit '0'
                    auto controlBits = boost::dynamic_bitset<>(1, 0);
                    res = concatDynBitSet(res, controlBits);
                }
                else
                {
                    // control bit '1'
                    auto controlBits = boost::dynamic_bitset<>(1, 1);
                    res = concatDynBitSet(res, controlBits);

                    //  5 bits for leading zeros
                    auto leadingZerosBits = boost::dynamic_bitset<>(5, leadingZeros);
                    res = concatDynBitSet(controlBits, leadingZerosBits);

                    //  6 bits for the length of the core part of the XOR
                    auto coreLengthBits = boost::dynamic_bitset<>(6, coreLength);
                    res = concatDynBitSet(res, coreLengthBits);
                }

                // Core bits of the XOR
                auto coreBits = boost::dynamic_bitset<>(coreLength, corePart);
                res = concatDynBitSet(res, coreBits);
            }
            result.push_back(res);
        }
        return result;
    }

    void compress(DataPoint *dp)
    {
        auto values = dp->values;
        auto ts = dp->timestamp;
        boost::dynamic_bitset<> time;
        boost::dynamic_bitset<> sequence;

        if (!HEADER)
            throw;

        if (FIRST_BLOCK)
        {
            auto delta = ts - header->timestamp;
            time = boost::dynamic_bitset<>(14, delta);
            auto outbits = valuesToBits(dp, header);
            FIRST_BLOCK = false;

            // outfile.write((char *)&outbits, sizeof(outbits));

            std::cout << sizeof(outbits) << std::endl;

            sec_last = header;
            last = dp;
        }
        else
        {
            int64_t d = (ts - last->timestamp) - (last->timestamp - sec_last->timestamp);
            time = deltaToBits(d);
            
            auto outbits = valuesToBits(dp, last);
            std::cout << sizeof(outbits) << std::endl;

            sec_last = last;
            last = dp;
        }

        outfile.close();
    }
};

// std::string pre = std::bitset<64>(*past).to_string();
// std::string cur = std::bitset<64>(*current).to_string();
// std::string xors = std::bitset<64>(xored).to_string();
// std::cout << pre << std::endl;
// std::cout << cur << std::endl;
// std::cout << values[i] << " ---- " << header->values[i] << std::endl;
// std::cout << xors << std::endl;

//  1 bit for control bit '1'
//  5 bits for leading zeros
//  6 bits for xor length
//  = 12

// std::cout << "Leading: " << leadingZeros << std::endl;
// std::cout << "Xor: " << blockValue << std::endl;
// std::cout << "Trailing: " << trailingZeros << std::endl;