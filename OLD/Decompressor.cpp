#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "DataPoint.cpp"

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
        DataPoint *head;
        
        rf.seekg (0);
        rf.read((char *)&head, sizeof(DataPoint));

        std::cout << head->values[0] << std::endl;
    };
};