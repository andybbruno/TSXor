#include <vector>
#include <iostream>
#include <string>
#include <fstream>

class Decompressor
{
private:
    std::string filename;
    std::ifstream rf;
    int nfeat;

public:
    Decompressor(std::string f, int n) : filename(f), nfeat(n)
    {
        rf = std::ifstream(filename, std::ios::out | std::ios::binary);
        if (!rf)
        {
            std::cout << "Unable to open file";
            exit(1); // terminate with error
        }
        std::vector<double> tmp(nfeat);
        for (int i = 0; i < 3; i++)
        {
            rf.read((char *)&tmp[i], nfeat * sizeof(double));
        }
    };
};