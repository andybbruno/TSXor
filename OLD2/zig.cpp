#include <iostream>
#include <limits.h>

inline uint64_t encodeZZ(int64_t i)
{
    return (i >> 63) ^ (i << 1);
}

inline int64_t decodeZZ(uint64_t i){
    return (i >> 1) ^ (-(i & 1));
}

int main()
{
    for (int64_t i = INT64_MIN; i < INT64_MAX; i+=100000)
    {
        uint64_t enc = (encodeZZ(i));
        int64_t dec = (decodeZZ(enc));
        assert(i == dec);
    }
    return 0;
}