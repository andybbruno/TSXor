#include <vector>
// #include <filesystem>
#include <chrono>
#include <numeric>
#include <iostream>
#include "../../core/DecompressorLZXOR.cpp"
#include "../../core/CompressorLZXOR.cpp"
#include "../../util/CSVReader.cpp"


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

    std::vector<std::vector<double>> data(nlines, std::vector<double>(ncols));

    for (int i = 0; i < nlines; i++)
    {
        for (int j = 0; j < ncols; j++)
        {
            infile.read((char *)&data[i][j], sizeof(double));
        }
    }
    infile.close();
    ncols--;

    std::vector<std::vector<double>> values;
    std::vector<uint64_t> times;

    for (auto x : data)
    {
        times.push_back(x[0]);
        std::vector<double> t(x.begin() + 1, x.end());
        values.push_back(t);
    }

    auto start_compr = std::chrono::system_clock::now();
    CompressorLZXOR c(times[0]);
    for (int i = 0; i < nlines; i++)
    {
        c.addValue(times[i], values[i]);
    }

    c.close();

    auto end_compr = std::chrono::system_clock::now();
    auto elapsed = (end_compr - start_compr);
    auto microsec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    auto original_size = 64 * nlines * (ncols + 1);
    auto compressed_size = c.bstream.size() + (c.bytes.size() * 8);
    std::cout.precision(3);
    std::cout << std::fixed;

    std::cout << "*** COMPRESSION ***" << std::endl;
    std::cout << "Computed in:         \t" << ((double)microsec / 1000) << " msec" << std::endl;
    std::cout << "Throughput 1:          \t" << (double)nlines / ((double)microsec) << " M Lines/s" << std::endl;
    std::cout << "Throughput 2:          \t" << ((double)(nlines * (ncols + 1)) / ((double)microsec)) << " M Value/s" << std::endl;
    std::cout << "Throughput 3:          \t" << ((double)(nlines * (ncols + 1)) / ((double)microsec)) * 8 << " MB/s" << std::endl;
    std::cout << "Reduction size: \t" << ((double)original_size / compressed_size) << "x" << std::endl
              << std::endl;

    auto outfile = std::ofstream("compressed_data.lzx", std::ios::out | std::ios::binary);

    auto bits_strm = c.bstream.data;
    auto bits_strm_msize = c.bstream.m_size;
    auto bytes_strm = c.bytes;

    auto bit_size = bits_strm.size();
    auto byte_size = bytes_strm.size();

    outfile.write((char *)&nlines, sizeof(uint64_t));
    outfile.write((char *)&ncols, sizeof(uint64_t));
    outfile.write((char *)&bit_size, sizeof(uint64_t));
    outfile.write((char *)&byte_size, sizeof(uint64_t));
    outfile.write((char *)&bits_strm_msize, sizeof(uint64_t));


    std::cout << nlines << std::endl;
    std::cout << ncols << std::endl;
    std::cout << bit_size << std::endl;
    std::cout << byte_size << std::endl;
    std::cout << bits_strm_msize << std::endl;



    outfile.write(reinterpret_cast<char *>(&bits_strm[0]),
                  bit_size * sizeof(uint64_t));

    outfile.write(reinterpret_cast<char *>(&bytes_strm[0]),
                  byte_size * sizeof(uint8_t));

    outfile.close();

    return 0;
}