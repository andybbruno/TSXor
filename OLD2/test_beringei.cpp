#include <vector>
#include <filesystem>
#include "Beringei.cpp"

int numLines = 0;

int main(int argc, char *argv[])
{
    long iterations = atoi(argv[1]);

    Beringei b(1, 1);
    srand(time(NULL));

    auto start = std::chrono::system_clock::now();

    for (int i = 1; i < iterations; i++)
    {
        b.append(i, rand() % 100);
    }

    auto end = std::chrono::system_clock::now();
    auto elapsed = (end - start);
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    auto original_size = 16 * iterations;
    auto compressed_size = b.bs.size_in_bytes();

    std::cout.precision(3);
    std::cout << std::fixed;
    std::cout << "Computed in:         \t" << msec << " msec" << std::endl;
    std::cout << "Throughput:          \t" << ((double)iterations / ((double)msec / 1000)) / 1000000 << " M DataPoint/s" << std::endl;
    std::cout << "Original size: \t\t" << ((double)original_size / 1000000) << " MB" << std::endl;
    std::cout << "Compressed size: \t" << ((double)compressed_size / 1000000) << " MB" << std::endl;
    std::cout << "Reduction size: \t" << ((double)original_size / compressed_size) << "x" << std::endl;

    return 0;
}