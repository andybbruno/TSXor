#include <vector>
#include <filesystem>
#include "Decompressor.cpp"
#include "DataPoint.cpp"

int main(int argc, char *argv[])
{
    std::string filename = argv[1];
    Decompressor(filename, 19);
    return 0;
}