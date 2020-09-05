#include <iostream>
#include <vector>
#include <string>
#include "CSVReader.cpp"

int main(int argc, char *argv[])
{
    std::string filename(argv[1]);
    int skip_timestamp = argc > 2 ? 1 : 0;

    CSVReader reader(filename);

    uint64_t nlines = 0;
    std::vector<std::vector<double>> lines;
    while (!reader.isEmpty())
    {
        lines.push_back(reader.nextLine());
        nlines++;
    }
    uint64_t ncols = lines[0].size();

    std::string rawname = filename.substr(0, filename.find_last_of("."));
    filename = rawname + std::string(".bin");

    auto myfile = std::ofstream(filename, std::ios::out | std::ios::binary);

    char *c_nlines = (char *)&nlines;
    char *c_ncols = (char *)&ncols;

    myfile.write(c_nlines, sizeof(uint64_t));
    myfile.write(c_ncols, sizeof(uint64_t));

    for (int i = 0; i < nlines; i++)
    {
        for (int j = skip_timestamp; j < ncols; j++)
        {
            myfile.write((char *)&lines[i][j], sizeof(double));
        }
    }

    myfile.close();

    std::cout << "File successfully converted!" << std::endl;
    std::cout << filename << std::endl;
}