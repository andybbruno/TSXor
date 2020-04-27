#include <vector>
#include <filesystem>
#include "CompressorMultiElias.cpp"
#include "CSVReader.cpp"

int numLines = 0;

int main(int argc, char *argv[])
{
    // if (argc < 2)
    // {
    //     return 0;
    // }

    // CSVReader reader(argv[1]);

    CSVReader reader("/Users/andrea/workspace/TimeSeries/dataset/Bari_full_UTC_UNIX.csv");

    // long lines = atoi(argv[1]);
    // int ncols = atoi(argv[2]);

    int nlines = 0;
    std::vector<std::vector<double>> lines;
    while (!reader.isEmpty())
    {
        lines.push_back(reader.nextLine());
        nlines++;
    }
    int ncols = lines[0].size() - 1;

    std::vector<std::vector<double>> values;
    std::vector<uint64_t> times;

    for (auto x : lines)
    {
        times.push_back(x[0]);
        std::vector<double> t(x.begin() + 1, x.end());
        values.push_back(t);
    }

    auto start = std::chrono::system_clock::now();

    CompressorMultiElias c(times[0] - 1);

    for (int i = 0; i < nlines; i++)
    {
        c.addValue(times[i], values[i]);
    }
    c.close();
    auto end = std::chrono::system_clock::now();
    auto elapsed = (end - start);
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    auto original_size = 64 * nlines * (ncols + 1);
    auto compressed_size = c.out.size();
    // std::cout << "Compressed size: \t" << (compressed_size) << " MB" << std::endl;

    // auto original_size = 8 * nlines * (ncols + 1);
    // auto compressed_size = c.bs.size();

    // auto ts_compression = ((double)(8 * nlines) / (c.ts_bits / 8));
    // auto data_compression = ((double)(8 * nlines * ncols) / ((c.bs.size() - c.ts_bits) / 8));

    std::cout.precision(3);
    std::cout << std::fixed;
    std::cout << "Computed in:         \t" << msec << " msec" << std::endl;
    std::cout << "Throughput:          \t" << ((double)nlines / ((double)msec / 1000)) / 1000000 << " M DataPoint/s" << std::endl;
    std::cout << "Original size: \t\t" << original_size << " Bits" << std::endl;
    std::cout << "Compressed size: \t" << compressed_size << " Bits" << std::endl;
    std::cout << "Reduction size: \t" << ((double)original_size / compressed_size) << "x" << std::endl;

    // std::cout << "Time Reduction:      \t" << ts_compression << "x" << std::endl;
    // std::cout << "Data Reduction: \t" << data_compression << "x" << std::endl;

    // for (auto x : c.map)
    // {
    //     std::cout << x.first << " - " << x.second << std::endl;
    // }

    return 0;
}