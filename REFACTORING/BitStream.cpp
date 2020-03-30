#include <deque>

struct BitStream
{
    std::deque<uint64_t> deque;
    uint tail_size;

    BitStream() : tail_size(0)
    {
        deque.push_back(0);
    };

    void append(uint64_t x, uint nbit)
    {
        if (nbit >= 64)
        {
            throw;
        }

        //TODO --> shift = nbits - #trailing_zeros

        auto x_zeros = __builtin_clzll(x);

        uint64_t tail = deque.back();

        if ((x == 0) && (tail_size < 64))
        {
            tail_size += nbit;
        }
        else if (tail == 0)
        {
            if (tail_size == 0)
            {
                deque[deque.size() - 1] = x << x_zeros;
                tail_size = 64 - x_zeros;
            }
            else if (0 < tail_size && tail_size < 64)
            {
                deque[deque.size() - 1] = x << (x_zeros - tail_size);
                tail_size += (64 - x_zeros);
            }
            else
            {
                throw;
            }
        }
        else
        {
            // size in bit of x ==> 64 - x_zeros
            // remaining space in bit in the tail ==> 64 - tail_size
            if ((64 - x_zeros) <= (64 - tail_size))
            {
                x <<= (x_zeros - tail_size);
                tail |= x;
                deque[deque.size() - 1] = tail;
                tail_size += (64 - x_zeros);
            }
        }

        if (tail_size == 64)
        {
            deque.push_back(0);
            tail_size -= 64;
        }
    }
};