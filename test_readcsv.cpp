#include <vector>
#include <filesystem>
#include "Compressor.cpp"
#include "CSVReader.cpp"
#include "DataPoint.cpp"

const int DataPoint::num_cols = 19;

int main()
{
    auto start = std::chrono::system_clock::now();
    std::string filename = "dataset/Bari_UTC_UNIX.csv";

    CSVReader reader(filename);

    Compressor *c;

    std::time_t tt = std::time(NULL);
    std::string s = std::ctime(&tt);

    if (!reader.isEmpty())
    {
        std::vector<double> values = reader.nextLine();
        auto time = values[0];
        values.erase(values.begin());
        c = new Compressor(new DataPoint(time, values), std::string(s+".bin"));
        // Decompressor("data.bin", 20);
    }

    while (!reader.isEmpty())
    {
        std::vector<double> values = reader.nextLine();
        auto time = values[0];
        values.erase(values.begin());
        c->compress(new DataPoint(time, values));
    }

    uintmax_t filesize;
    try
    {
        filesize = std::filesystem::file_size(filename);
    }
    catch (std::filesystem::filesystem_error &ex)
    {
        std::cout << ex.what() << '\n';
    }

    auto end = std::chrono::system_clock::now();
    auto elapsed = end - start;
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();

    std::cout << "Computed in: " << msec << "msec" << std::endl;
    std::cout << "Throughput: " << double(((filesize / 1024 / 1024) / sec)) << "MB/s" << std::endl;

    return 0;
}