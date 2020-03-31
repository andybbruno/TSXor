#include <vector>
#include <filesystem>
#include "Compressor.cpp"
#include "CSVReader.cpp"
#include "DataPoint.cpp"

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
        c = new Compressor (time, values);
        // Decompressor("data.bin", 20);
    }

    std::vector<std::vector<double>> lines;
    while (!reader.isEmpty())
    {
        lines.push_back(reader.nextLine());
        numLines++;
    }

    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < lines.size(); i++)
    {
        auto val = lines[i];
        auto time = val[0];
        val.erase(val.begin());
        c->append(time, val);
    }

    auto end = std::chrono::system_clock::now();
    
    auto compr_bits = c->bs.size();
    auto num_cols = c->num_cols;
    auto elapsed = end - start;
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    auto num_points = numLines * (num_cols + 1);
    // 8 bytes per point (double)
    auto estimated_original_size = 8 * num_points;
    auto estimated_compressed_size = compr_bits / 8;
    auto compr_ratio = ((double)estimated_original_size / (estimated_compressed_size));

    auto bytesperline = ((double)estimated_compressed_size / numLines);
    std::cout << bytesperline << std::endl;

    std::cout.precision(3);
    std::cout << std::fixed;
    std::cout << "Computed in: " << msec << " msec" << std::endl;
    std::cout << "Throughput: " << ((double)(numLines * num_cols) / ((double)msec / 1000)) << " DataPoint/s" << std::endl;
    std::cout << "Original size: \t\t" << estimated_original_size << std::endl;
    std::cout << "Compressed size: \t" << estimated_compressed_size << std::endl;
    // std::cout << "Compression Ratio:" << compr_ratio << std::endl;
    // std::cout << "Size reduction:" << 100 - compr_ratio << "%" << std::endl;

    return 0;
}