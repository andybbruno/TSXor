#include <deque>
#include <iostream>

struct BitStream
{
    std::deque<bool> deque;

    void addBit(bool b)
    {
        deque.push_front(b);
    }

    // void append(uint64_t x)
    // {
    //     std::deque<bool> v;
    //     auto nzeros = __builtin_clzll(x);
    //     for (size_t i = 0; i < (64 - nzeros); i++)
    //     {
    //         (x & 1) ? v.push_front(1) : v.push_front(0);
    //         x >>= 1;
    //     }
    //     deque.insert(deque.end(), v.begin(), v.end());
    // }

    // void addValue(uint64_t x, size_t nbit, std::string caller)
    void addValue(uint64_t x, size_t nbit)
    {
        if (x == 0)
        {
            for (int i = 0; i < nbit; i++)
            {
                deque.push_front(0);
            }
            return;
        }

        // std::deque<bool> v;

        auto length = 64 - __builtin_clzll(x);

        if (nbit < length)
        {
            // std::cout << caller << std::endl;
            std::cout << "Can't compress " << x << " using only " << nbit << " bits." << std::endl;
            throw;
        }

        if (nbit > 64)
        {
            // std::cout << caller << std::endl;
            std::cout << "Too many bits." << std::endl;
            throw;
        }
        
        // for (size_t i = 0; i < (length + nbit); i++)
        // {
        //     (x & 1) ? deque.push_front(1) : deque.push_front(0);
        //     x >>= 1;
        // }
        // deque.insert(deque.end(), v.begin(), v.end());
    }
};