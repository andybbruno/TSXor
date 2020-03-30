#include <iostream>
#include "BitStream.cpp"
int main()
{

    BitStream stream;

    // uint64_t a = 0b1000000000000000000000000000000000000000000000000000000000000000;
    // uint64_t b = a++;

    // for (int i = 0; i < 64; i++)
    // {
    //     stream.append(0, 65);
    // }
    stream.append(0,65);
    // stream.append(5);
    // stream.append(4);
    // stream.append(0);
    // stream.append(1);

    // auto leading_zeros_a = __builtin_clzll(a);
    // auto lenght_a = 64 - leading_zeros_a;

    // auto leading_zeros_b = __builtin_clzll(b);
    // auto shift_b = leading_zeros_b - lenght_a;

    // a = a << leading_zeros_a;
    // std::cout << std::hex << a << std::endl;

    // b = b << shift_b;
    // std::cout << std::hex << b << std::endl;

    // a = a | b;
    // std::cout << std::hex << a << std::endl;
}
