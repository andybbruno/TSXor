#include <vector>
#include <filesystem>
#include "Compressor.cpp"

int numLines = 0;

int main(int argc, char *argv[])
{
    // long lines = 100000000;
    // int ncols = 20;
    long lines = atoi(argv[1]);
    int ncols = atoi(argv[2]);

    auto t0 = 9468576;

    srand(time(NULL));

    std::vector<std::vector<double>> values;
    std::vector<uint64_t> times;

    std::vector<double> last(ncols, rand());

    for (int i = 0; i < lines; i++)
    {
        times.push_back(i);

        std::vector<double> tmp;
        for (int j = 0; j < ncols; j++)
        {
            int sign = rand() % 2;
            double val;
            if (sign == 0)
            {
                val = (i == 0) ? (rand() % 100) : last[j] + (rand() % 8);
            }
            else
            {
                val = (i == 0) ? (rand() % 100) : last[j] - (rand() % 8);
            }
            tmp.push_back(val);
        }
        last = tmp;
        values.push_back(tmp);
    }

    auto start = std::chrono::system_clock::now();

    Compressor c(times[0], values[0]);

    for (int i = 1; i < lines; i++)
    {
        c.append(times[i], values[i]);
    }

    auto end = std::chrono::system_clock::now();
    auto elapsed = (end - start);
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    auto original_size = 8 * lines * (ncols + 1);
    auto compressed_size = c.bs.size_in_bytes();

    auto ts_compression = ((double)(8 * lines) / (c.ts_bits / 8));
    auto data_compression = ((double)(8 * lines * ncols) / ((c.bs.size() - c.ts_bits) / 8));

    std::cout.precision(3);
    std::cout << std::fixed;
    std::cout << "Computed in:         \t" << msec << " msec" << std::endl;
    std::cout << "Throughput:          \t" << ((double)lines / ((double)msec / 1000)) / 1000000 << " M DataPoint/s" << std::endl;
    std::cout << "Original size: \t\t" << ((double)original_size / 1000000) << " MB" << std::endl;
    std::cout << "Compressed size: \t" << ((double)compressed_size / 1000000) << " MB" << std::endl;
    std::cout << "Reduction size: \t" << ((double)original_size / compressed_size) << "x" << std::endl;
    std::cout << "Time Reduction:      \t" << ts_compression << "x" << std::endl;
    std::cout << "Data Reduction: \t" << data_compression << "x" << std::endl;
    return 0;
}