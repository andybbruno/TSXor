#include <vector>
#include <filesystem>
#include "lib/CompressorMulti.cpp"
#include "lib/DecompressorMulti.cpp"
#include "lib/CSVReader.cpp"

int numLines = 0;

int main(int argc, char *argv[])
{

    // CSVReader reader("/Users/andrea/workspace/TimeSeries/dataset/globalterrorism_UTC_UNIX.csv");

    if (argc < 2)
    {
        return 0;
    }

    CSVReader reader(argv[1]);

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

    auto start_compr = std::chrono::system_clock::now();
    CompressorMulti c(times[0]);
    for (int i = 0; i < nlines; i++)
    {
        c.addValue(times[i], values[i]);
    }

    c.close();

    auto end_compr = std::chrono::system_clock::now();
    auto elapsed = (end_compr - start_compr);
    auto msec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    auto original_size = 64 * nlines * (ncols + 1);
    auto compressed_size = c.out.size();
    std::cout.precision(3);
    std::cout << std::fixed;
    std::cout << "*** COMPRESSION ***" << std::endl;
    std::cout << "Computed in:         \t" << ((double)msec / 1000) << " msec" << std::endl;
    std::cout << "Throughput:          \t" << ((double)nlines / ((double)msec / 1000000)) / 1000000 << " M DataPoint/s" << std::endl;
    // std::cout << "Original size: \t\t" << original_size << " Bits" << std::endl;
    // std::cout << "Compressed size: \t" << compressed_size << " Bits" << std::endl;
    std::cout << "Reduction size: \t" << ((double)original_size / compressed_size) << "x" << std::endl
              << std::endl;

    auto start_dec = std::chrono::system_clock::now();
    DecompressorMulti dm(c.out, ncols);
    while (dm.hasNext())
    {
        // PairMulti p = dm.readPair();
        // std::cout << p.toString() << std::endl;
        // std::cout << dm.in << std::endl;
    }
    auto end_dec = std::chrono::system_clock::now();
    elapsed = (end_dec - start_dec);
    msec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout << "*** DECOMPRESSION ***" << std::endl;
    std::cout << "Computed in:         \t" << ((double)msec / 1000) << " msec" << std::endl;
    std::cout << "Throughput:          \t" << ((double)nlines / ((double)msec / 1000000)) / 1000000 << " M DataPoint/s" << std::endl
              << std::endl;

    std::cout << "*** LAST ROW ***" << std::endl;
    std::cout << dm.storedTimestamp << " -> ";
    for (auto x : dm.storedVal)
        std::cout << x << "|";
    std::cout << std::endl;
    return 0;
}