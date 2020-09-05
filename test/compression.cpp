#include <vector>
// #include <filesystem>
#include <chrono>
#include <iostream>
#include <numeric>
#include "../core/DecompressorTSXor.cpp"
#include "../core/CompressorTSXor.cpp"
#include "../util/CSVReader.cpp"

int numLines = 0;

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        return 0;
    }

    bool printAsCSV = argc > 2;
    std::string filename(argv[1]);
    auto infile = std::ifstream(filename, std::ios::out | std::ios::binary);

    uint64_t nlines;
    uint64_t ncols;

    infile.read((char *)&nlines, sizeof(uint64_t));
    infile.read((char *)&ncols, sizeof(uint64_t));

    std::vector<std::vector<double>> values(nlines, std::vector<double>(ncols));

    for (int i = 0; i < nlines; i++)
    {
        for (int j = 0; j < ncols; j++)
        {
            infile.read((char *)&values[i][j], sizeof(double));
        }
    }
    infile.close();

    auto start_compr = std::chrono::system_clock::now();
    CompressorTSXor c(values[0]);
    for (int i = 0; i < nlines; i++)
    {
        c.addValue(values[i]);
    }

    c.close();

    auto end_compr = std::chrono::system_clock::now();
    auto elapsed = (end_compr - start_compr);
    auto microsec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    auto original_size = 64 * nlines * ncols;
    auto compressed_size = c.bytes.size() * 8;
    std::cout.precision(3);
    std::cout << std::fixed;

    std::cout << "*** COMPRESSION ***" << std::endl;
    std::cout << "Computed in:         \t" << ((double)microsec / 1000) << " msec" << std::endl;
    std::cout << "Throughput 1:          \t" << (double)nlines / ((double)microsec) << " M Lines/s" << std::endl;
    std::cout << "Throughput 2:          \t" << ((double)(nlines * ncols) / ((double)microsec)) << " M Value/s" << std::endl;
    std::cout << "Throughput 3:          \t" << ((double)(nlines * ncols) / ((double)microsec)) * 8 << " MB/s" << std::endl;
    std::cout << "Reduction size: \t" << ((double)original_size / compressed_size) << "x" << std::endl
              << std::endl;

    auto outfile = std::ofstream("compressed_data.tsx", std::ios::out | std::ios::binary);


    auto byte_size = c.bytes.size();

    outfile.write((char *)&nlines, sizeof(uint64_t));
    outfile.write((char *)&ncols, sizeof(uint64_t));
    outfile.write((char *)&byte_size, sizeof(uint64_t));

    outfile.write(reinterpret_cast<char *>(&c.bytes[0]),
                  byte_size * sizeof(uint8_t));

    outfile.close();

    return 0;
}