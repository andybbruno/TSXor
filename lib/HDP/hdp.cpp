#pragma once
#include <vector>
#include <deque>
#include <algorithm>

class HDP
{
public:
    std::vector<std::deque<uint64_t>> table;

    HDP()
    {
        table = std::vector<std::deque<uint64_t>>(8, std::deque<uint64_t>(1, 0));
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

        uint64_t zeros = (leadingZerosBytes << 4) | trailingZerosBytes;

        return std::pair(zeros, xor_);
    }

    // double decode(bool usefcm, uint64_t body)
    // {
    // }

    // void head(double d)
    // {
    // }

private:
    uint64_t getBestPrediction(uint64_t true_value)
    {

        int leadingZerosBytes = 0;
        int trailingZerosBytes = 0;

        if (true_value != 0)
        {
            leadingZerosBytes = __builtin_clzll(true_value) / 8;
            trailingZerosBytes = __builtin_ctzll(true_value) / 8;
        }

        auto bucket = leadingZerosBytes + trailingZerosBytes;

        // if current bucket not empty
        if (table[bucket].size() > 0)
        {
            std::vector<uint64_t> res(table[bucket].size());
            for (int i = 0; i < table[bucket].size(); i++)
            {
                auto tmp_xor = table[bucket][i] ^ true_value;
                res[i] = (__builtin_popcountll(tmp_xor)) / 8;
            }
            auto minidx = std::distance(res.begin(), std::min_element(res.begin(), res.end()));

            update(true_value, bucket);
            return table[bucket][minidx];
        }

        update(true_value, bucket);
        return 0;
    }

    // inline uint64_t getBestPrediction(uint64_t true_value)
    // {
    //     auto popcount = __builtin_popcountll(true_value);

    //     // if current bucket not empty
    //     if (table[popcount].size() > 0)
    //     {
    //         std::vector<uint64_t> res(table[popcount].size());
    //         for (int i = 0; i < table[popcount].size(); i++)
    //         {
    //             auto xor_ = table[popcount][i] ^ true_value;
    //             int leadingZerosBytes = safeLeadingZerosBytes(xor_);
    //             int trailingZerosBytes = safeTrailingZerosBytes(xor_);
    //             res[i] = leadingZerosBytes + trailingZerosBytes;
    //         }
    //         auto maxidx = std::distance(res.begin(), std::max_element(res.begin(), res.end()));

    //         update(true_value, popcount);

    //         return table[popcount][maxidx];
    //     }
    //     //for i da popcount +/- threshold
    //     // else
    //     // {
    //     // }

    //     return 0;
    // }

    inline void update(uint64_t val, int bucket)
    {
        if (table[bucket].size() > 15)
        {
            table[bucket].pop_back();
        }
        table[bucket].push_front(val);
    }
};