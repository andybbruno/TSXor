#include <vector>
// #include <filesystem>
#include <chrono>
#include <iostream>
#include <numeric>
#include "../core/DecompressorLZXOR.cpp"
#include "../core/CompressorLZXOR.cpp"
#include "../util/CSVReader.cpp"

int numLines = 0;

int main(int argc, char *argv[])
{
    auto infile = std::ifstream("compressed_data.lzx", std::ios::out | std::ios::binary);

    uint64_t nlines;
    uint64_t ncols;
    uint64_t bit_size;
    uint64_t byte_size;
    uint64_t bitstream_m_size;

    infile.read((char *)&nlines, sizeof(uint64_t));
    infile.read((char *)&ncols, sizeof(uint64_t));
    infile.read((char *)&bit_size, sizeof(uint64_t));
    infile.read((char *)&byte_size, sizeof(uint64_t));
    infile.read((char *)&bitstream_m_size, sizeof(uint64_t));

    std::deque<uint64_t> in_bits(bit_size);
    std::vector<uint8_t> in_bytes(byte_size);

    infile.read(reinterpret_cast<char *>(&in_bits[0]),
                bit_size * sizeof(uint64_t));
    
    infile.read(reinterpret_cast<char *>(&in_bytes[0]),
                byte_size * sizeof(uint8_t));

    BitStream bits(in_bits, bitstream_m_size);

    auto start_dec = std::chrono::system_clock::now();
    DecompressorLZXOR dm(bits, in_bytes, ncols);

    while (dm.hasNext()){}

    auto end_dec = std::chrono::system_clock::now();
    auto elapsed = (end_dec - start_dec);
    auto microsec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    std::cout << "*** DECOMPRESSION ***" << std::endl;
    std::cout << "Computed in:         \t" << ((double)microsec / 1000) << " msec" << std::endl;
    std::cout << "Throughput 1:          \t" << (double)nlines / ((double)microsec) << " M Lines/s" << std::endl;
    std::cout << "Throughput 2:          \t" << ((double)(nlines * (ncols + 1)) / ((double)microsec)) << " M Value/s" << std::endl;
    std::cout << "Throughput 3:          \t" << ((double)(nlines * (ncols + 1)) / ((double)microsec)) * 8 << " MB/s" << std::endl;

    std::cout.precision(6);
    std::cout << std::fixed;
    std::cout << "*** LAST ROW ***" << std::endl;
    std::cout << dm.storedTimestamp << " -> ";
    for (auto x : dm.storedVal)
        std::cout << x << "|";
    std::cout << std::endl;

    return 0;
}