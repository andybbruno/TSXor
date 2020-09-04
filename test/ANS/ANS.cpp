#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <numeric>
#include <math.h>

int encode(char s, std::map<char, int> &freqs, int state = 0)
{
    std::map<char, int> cumsum;
    int sum = 0;

    for (auto c : freqs)
    {
        cumsum.insert(std::pair(c.first, sum));
        sum += c.second;
    }

    int s_freq = freqs.at(s);
    int next_state = floor(state / s_freq) * sum + cumsum.at(s) + (state % s_freq);
    return next_state;
}

int decode()
{
}

int main(int argc, char *argv[])
{
    std::map<char, int> freqs;

    freqs.insert(std::pair('a', 3));
    freqs.insert(std::pair('b', 3));
    freqs.insert(std::pair('c', 3));

    std::string s("abac");

    int state = 0;
    for (auto c : s)
    {
        state = encode(c, freqs, state);
    }

    std::cout << "Final State: " << state << std::endl;
}
