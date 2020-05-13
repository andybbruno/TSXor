#pragma once
#include <deque>
#include <unordered_set>

template <class T>
class Cache
{
private:
    std::deque<T> buffer;
    std::unordered_set<T> set;

public:
    Cache(size_t dim = 128)
    {
        buffer = std::deque<T>(dim, 0);
    }

    void insert(T val)
    {
        //Remove tail from both containers
        auto remove_tail = buffer.back();
        set.erase(remove_tail);
        buffer.pop_back();

        //Add val to both
        buffer.push_front(val);
        set.insert(val);
    }

    bool contains(T val)
    {
        return set.find(val) != set.end();
    }

    int getIndexOf(T val)
    {
        return std::distance(buffer.begin(), std::find(buffer.begin(), buffer.end(), val));
    }

    T get(int offset)
    {
        return buffer[offset];
    }

    T getLast()
    {
        return buffer.front();
    }
};