#include<iostream> 
#include<limits.h>

//ZIGZAG ENCODING/DECODING FOR 32 BITS
#define encodeZZ(i) (i >> 31) ^ (i << 1)
#define decodeZZ(i) (i << 1) ^ (i >> 31)

int main()
{
    for (int i = INT_MIN; i < INT_MAX; i++)
    {   
        int enc = (encodeZZ(i));
        int dec = (decodeZZ(i));
        assert(enc == dec);
    }
    return 0;
}