#include <vector>
#include <filesystem>
#include <numeric>
#include "core/CompressorXorCache.cpp"
#include "core/DecompressorXorCache.cpp"
#include "lib/CSVReader.cpp"

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
    CompressorXorCache c(times[0]);
    for (int i = 0; i < nlines; i++)
    {
        c.addValue(times[i], values[i]);
    }

    c.close();

    auto end_compr = std::chrono::system_clock::now();
    auto elapsed = (end_compr - start_compr);
    auto microsec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    auto original_size = 64 * nlines * (ncols + 1);
    auto compressed_size = c.out.size() + (c.bytes.size() * 8);
    std::cout.precision(3);
    std::cout << std::fixed;

    if (printAsCSV)
    {
        std::cout << ((double)microsec / 1000) << ","
                  << (double)nlines / ((double)microsec) << ","
                  << ((double)(nlines * (ncols + 1)) / ((double)microsec)) << ",";
    }
    else
    {
        std::cout << "*** COMPRESSION ***" << std::endl;
        std::cout << "Computed in:         \t" << ((double)microsec / 1000) << " msec" << std::endl;
        std::cout << "Throughput 1:          \t" << (double)nlines / ((double)microsec) << " M Lines/s" << std::endl;
        std::cout << "Throughput 2:          \t" << ((double)(nlines * (ncols + 1)) / ((double)microsec)) << " M Value/s" << std::endl;
        // std::cout << "Original size: \t\t" << original_size << " Bits" << std::endl;
        // std::cout << "Compressed size: \t" << compressed_size << " Bits" << std::endl;
        std::cout << "Reduction size: \t" << ((double)original_size / compressed_size) << "x" << std::endl
                  << std::endl;
    }

    auto start_dec = std::chrono::system_clock::now();
    DecompressorXorCache dm(c.out, c.bytes, ncols);
    while (dm.hasNext())
    {
    }
    auto end_dec = std::chrono::system_clock::now();
    elapsed = (end_dec - start_dec);
    microsec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    if (printAsCSV)
    {
        std::cout << ((double)microsec / 1000) << ","
                  << (double)nlines / ((double)microsec) << ","
                  << ((double)(nlines * (ncols + 1)) / ((double)microsec)) << std::endl;
    }
    else
    {
        std::cout << "*** DECOMPRESSION ***" << std::endl;
        std::cout << "Computed in:         \t" << ((double)microsec / 1000) << " msec" << std::endl;
        std::cout << "Throughput 1:          \t" << (double)nlines / ((double)microsec) << " M Lines/s" << std::endl;
        std::cout << "Throughput 2:          \t" << ((double)(nlines * (ncols + 1)) / ((double)microsec)) << " M Value/s" << std::endl
                  << std::endl;

        std::cout.precision(6);
        std::cout << std::fixed;
        std::cout << "*** LAST ROW ***" << std::endl;
        std::cout << dm.storedTimestamp << " -> ";
        for (auto x : dm.storedVal)
            std::cout << x << "|";
        std::cout << std::endl;

                std::cout.precision(2);
        std::cout << std::fixed;
        std::cout << "A: \t" << ((double)c.countA / (nlines * ncols)) * 100 << "%" << std::endl;
        std::cout << "B: \t" << ((double)c.countB / (nlines * ncols)) * 100 << "%" << std::endl;
        std::cout << "C: \t" << ((double)c.countC / (nlines * ncols)) * 100 << "%" << std::endl;
        std::cout << "B  -->  " << ((double)c.countB_bytes / (c.countB)) << " bytes" << std::endl;
    }

    return 0;
}