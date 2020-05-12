#include <vector>
#include <filesystem>
#include "lib/CompressorHDP.cpp"
// #include "lib/DecompressorFPC.cpp"
#include "lib/CSVReader.cpp"

int numLines = 0;

int main(int argc, char *argv[])
{

    // CSVReader reader("/Users/andrea/workspace/TimeSeries/csv/_DEBUG.csv");
    // CSVReader reader("dataset/OXFORD/manrealizedvolatilityindices_UTC_UNIX_lbl.csv");
    // CSVReader reader("csv/test_2COLS_100K.csv");

    if (argc < 2)
    {
        return 0;
    }

    CSVReader reader(argv[1]);
    bool printAsCSV = argc > 2;

    // long lines = atoi(argv[1]);
    // int ncols = atoi(argv[2]);

    uint64_t nlines = 0;
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
    CompressorHDP c(times[0]);
    for (int i = 0; i < nlines; i++)
    {
        c.addValue(times[i], values[i]);
    }

    c.close();

    auto end_compr = std::chrono::system_clock::now();
    auto elapsed = (end_compr - start_compr);
    auto microsec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    auto original_size = 64 * nlines * (ncols + 1);
    auto compressed_size = c.out.size();
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

    // auto start_dec = std::chrono::system_clock::now();
    // DecompressorFPC dm(c.out, ncols);
    // while (dm.hasNext())
    // {
    //     // PairMulti p = dm.readPair();
    //     // std::cout << p.toString() << std::endl;
    //     // std::cout << dm.in << std::endl;
    // }
    // auto end_dec = std::chrono::system_clock::now();
    // elapsed = (end_dec - start_dec);
    // microsec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    // if (printAsCSV)
    // {
    //     std::cout << ((double)microsec / 1000) << ","
    //               << (double)nlines / ((double)microsec) << ","
    //               << ((double)(nlines * (ncols + 1)) / ((double)microsec)) << std::endl;
    // }
    // else
    // {
    //     std::cout << "*** DECOMPRESSION ***" << std::endl;
    //     std::cout << "Computed in:         \t" << ((double)microsec / 1000) << " msec" << std::endl;
    //     std::cout << "Throughput 1:          \t" << (double)nlines / ((double)microsec) << " M Lines/s" << std::endl;
    //     std::cout << "Throughput 2:          \t" << ((double)(nlines * (ncols + 1)) / ((double)microsec)) << " M Value/s" << std::endl
    //               << std::endl;

    //     std::cout.precision(6);
    //     std::cout << std::fixed;
    //     std::cout << "*** LAST ROW ***" << std::endl;
    //     std::cout << dm.storedTimestamp << " -> ";
    //     for (auto x : dm.storedVal)
    //         std::cout << x << "|";
    //     std::cout << std::endl;
    // }

    std::cout << ((double)c.counter1 / c.counter0) * 100 << std::endl;
    std::cout << ((double)c.counter2 / c.counter0) * 100 << std::endl;
    std::cout << ((double)c.counter3 / c.counter0) * 100 << std::endl;

    return 0;
}