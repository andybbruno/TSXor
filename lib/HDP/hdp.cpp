#pragma once
#include <vector>
#include <deque>
#include <algorithm>

#define SHARED_BYTES_MASK 0x88
#define NO_SHARED_BYTES_MASK 0x80

class HDP
{
public:
    std::vector<std::deque<uint64_t>> table;
    std::deque<uint64_t> memory;

    HDP()
    {
        table = std::vector<std::deque<uint64_t>>(8, std::deque<uint64_t>(1, 0));
        memory = std::deque<uint64_t>(128, 0);
    }

    std::pair<uint64_t, uint64_t> encode(double d)
    {
        uint64_t d_int = *((uint64_t *)&d);
        auto best = getBestPrediction(d_int);
        auto xor_ = best ^ d_int;

        int leadingZerosBytes = 0;
        int trailingZerosBytes = 0;

        if (xor_ != 0)
        {
            leadingZerosBytes = __builtin_clzll(xor_) / 8;
            trailingZerosBytes = __builtin_clzll(xor_) / 8;
        }
        else
        {
            int index = std::distance(memory.begin(), std::find(memory.begin(), memory.end(), d_int));
            updateMemory(d_int);
            return std::pair(index, 0);
        }

        uint64_t zeros = SHARED_BYTES_MASK;

        if ((leadingZerosBytes > 0) || (trailingZerosBytes > 0))
        {
            zeros |= (leadingZerosBytes << 4);
            zeros |= trailingZerosBytes;
        }
        else
        {
            zeros = NO_SHARED_BYTES_MASK;
        }
        updateMemory(d_int);
        return std::pair(zeros, xor_);
    }

    // double decode(bool usefcm, uint64_t body)
    // {

    // }

    // void head(double d)
    // {
    // }

private:
    inline uint64_t getBestPrediction(uint64_t true_value)
    {
        int leadingZerosBytes = 0;
        int trailingZerosBytes = 0;

        if (true_value != 0)
        {
            leadingZerosBytes = __builtin_clzll(true_value) / 8;
            trailingZerosBytes = __builtin_ctzll(true_value) / 8;
        }

        auto bucket = leadingZerosBytes + trailingZerosBytes;

        uint64_t result = 0;

        // if current bucket not empty
        if (table[bucket].size() > 0)
        {
            std::vector<uint64_t> res(table[bucket].size());
            for (int i = 0; i < table[bucket].size(); i++)
            {
                auto tmp_xor = table[bucket][i] ^ true_value;
                
                // LOOK HERE!
                // res[i] = (__builtin_popcountll(tmp_xor)) / 8;
                res[i] = (__builtin_popcountll(tmp_xor));
            }
            auto minidx = std::distance(res.begin(), std::min_element(res.begin(), res.end()));
            result = table[bucket][minidx];
        }

        updateTable(true_value, bucket);
        return result;
    }

    inline void updateTable(uint64_t val, int bucket)
    {
        // LOOK HERE!
        if (table[bucket].size() > 7)
        {
            table[bucket].pop_back();
        }
        table[bucket].push_front(val);
    }

    inline void updateMemory(uint64_t val)
    {
        memory.pop_back();
        memory.push_front(val);
    }
};