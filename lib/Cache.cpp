#pragma once
#include <deque>
#include <unordered_set>

template <class T>
class Cache
{
private:
    std::deque<T> buffer;
    // std::unordered_set<T> set;
    // std::map<uint16_t, uint64_t> map;

    // inline uint16_t computeKey(T val)
    // {
    //     uint16_t key = 0;
    //     uint8_t *p = (uint8_t *)&val;

    //     key |= p[7];
    //     key <<= 8;
    //     // key |= p[6];
    //     // key <<= 8;
    //     // key |= p[1];
    //     // key <<= 8;
    //     key |= p[0];

    //     return key;
    // }

public:
    Cache(size_t dim = 64)
    {
        buffer = std::deque<T>(dim, 0);
    }

    void insert(T val)
    {
        //Remove tail from all containers
        auto remove_tail = buffer.back();
        // set.erase(remove_tail);
        // map.erase(computeKey(remove_tail));
        buffer.pop_back();

        //Add the fresh value to all containers
        buffer.push_front(val);
        // set.insert(val);
        // auto key = computeKey(val);
        // map.insert_or_assign(key, val);
    }

    inline bool contains(T val)
    {
        // return set.find(val) != set.end();
        return std::find(buffer.begin(), buffer.end(), val) != buffer.end();
    }

    inline int getIndexOf(T val)
    {
        return std::distance(buffer.begin(), std::find(buffer.begin(), buffer.end(), val));
    }

    inline T get(int offset)
    {
        return buffer[offset];
    }

    inline T getLast()
    {
        return buffer.front();
    }

    // inline bool containsCandidate(T val)
    // {
    //     auto key = computeKey(val);
    //     return map.find(key) != map.end();
    // }

    inline T getCandidate(T val)
    {
        // auto key = computeKey(val);
        // return map[key];
        std::vector<uint64_t> tmp(buffer.size(), 0);

        for (int i = 0; i < buffer.size(); i++)
        {
            tmp[i] = val ^ buffer[i];
        }

        std::transform(tmp.begin(),
                       tmp.end(),
                       tmp.begin(),
                       [&](uint64_t x) {
                           if (x != 0)
                           {
                               return __builtin_clzll(x) + __builtin_ctzll(x);
                           }
                           else
                           {
                               return 64;
                           }
                       });

        auto maxid = std::distance(tmp.begin(), std::max_element(tmp.begin(), tmp.end()));
        return buffer[maxid];
    }
};