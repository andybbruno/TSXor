#include <vector>
#include <filesystem>
#include "Compressor.cpp"
#include "CSVReader.cpp"
#include "DataPoint.cpp"

const int DataPoint::num_cols = 19;
int numLines = 0;

int main(int argc, char *argv[])
{
    auto start = std::chrono::system_clock::now();
    std::string filename = argv[1]; // "dataset/test.csv"
    std::string out_filename = argv[2]; // "RES.bin"

    CSVReader reader(filename);

    Compressor *c;

    std::time_t tt = std::time(NULL);
    std::string s = std::ctime(&tt);

    if (!reader.isEmpty())
    {
        std::vector<double> values = reader.nextLine();
        numLines++;
        auto time = values[0];
        values.erase(values.begin());
        // c = new Compressor(new DataPoint(time, values), std::string(s+".bin"));
        c = new Compressor(new DataPoint(time, values), out_filename);
        // Decompressor("data.bin", 20);
    }

    while (!reader.isEmpty())
    {
        std::vector<double> values = reader.nextLine();
        numLines++;
        auto time = values[0];
        values.erase(values.begin());
        c->compress(new DataPoint(time, values));
    }

    c->close();

    uintmax_t original_filesize;
    uintmax_t compressed_filesize;

    original_filesize = std::filesystem::file_size(filename);
    compressed_filesize = std::filesystem::file_size(out_filename);

    auto end = std::chrono::system_clock::now();
    auto elapsed = end - start;
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    std::cout.precision(3);
    std::cout << std::fixed;
    std::cout << "Computed in: " << msec << " msec" << std::endl;
    std::cout << "Throughput: " << ((double)numLines / msec * 1000) << " DataPoint/s" << std::endl;

    // std::cout << "Throughput: " << (((double)original_filesize / 1024 / 1024) / msec * 1000) << " MB/s" << std::endl;
    // std::cout << "Reduction: " << (1 - ((double)original_filesize / compressed_filesize)) * 100 << "%" << std::endl;
    // std::cout << "Compression Ratio:" << (double(8 * numLines * DataPoint::num_cols) / (compressed_filesize)) << std::endl;

    return 0;
}