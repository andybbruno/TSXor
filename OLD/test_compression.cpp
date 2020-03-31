#include <vector>
#include <filesystem>
#include "Compressor.cpp"
#include "CSVReader.cpp"
#include "DataPoint.cpp"

const int DataPoint::num_cols = 19;
int numLines = 0;

int main(int argc, char *argv[])
{
    std::string filename = argv[1];     // "dataset/test.csv"
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

    std::vector<std::vector<double>> lines;
    while (!reader.isEmpty())
    {
        lines.push_back(reader.nextLine());
        numLines++;
    }

    auto start = std::chrono::system_clock::now();
    for (auto val : lines)
    {
        auto time = val[0];
        val.erase(val.begin());
        c->compress(new DataPoint(time, val));
    }

    auto compr_bits = c->numbits;

    auto end = std::chrono::system_clock::now();
    auto elapsed = end - start;
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    auto num_points = numLines * DataPoint::num_cols;
    // 8 bytes per point (double)
    auto estimated_original_size = 8 * num_points;
    auto estimated_compressed_size = compr_bits / 8;
    auto compr_ratio = ((double)estimated_original_size / (estimated_compressed_size));

    std::cout.precision(3);
    std::cout << std::fixed;
    std::cout << "Computed in: " << msec << " msec" << std::endl;
    std::cout << "Throughput: " << ((double)(numLines * DataPoint::num_cols) / ((double)msec / 1000)) << " DataPoint/s" << std::endl;
    // std::cout << "Compression Ratio:" << compr_ratio << std::endl;
    // std::cout << "Size reduction:" << 100 - compr_ratio << "%" << std::endl;

    return 0;
}