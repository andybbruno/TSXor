#include <iostream>
#include <vector>
#include <string>
#include "lib/CSVReader.cpp"

int main(int argc, char *argv[])
{
    std::string filename(argv[1]);
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
        for (int j = 0; j < ncols; j++)
        {
            myfile.write((char *)&lines[i][j], sizeof(double));
        }
    }

    myfile.close();

    std::cout << "Closed" << std::endl;

    // READ PART
    // auto infile = std::ifstream(filename, std::ios::out | std::ios::binary);

    // uint64_t new_nclos;
    // uint64_t new_nlines;

    // infile.read((char *)&new_nlines, sizeof(uint64_t));
    // infile.read((char *)&new_nclos, sizeof(uint64_t));

    // std::cout << new_nclos << " --- " << new_nlines << std::endl;
    // std::vector<std::vector<double>> data(new_nlines, std::vector<double>(new_nclos));

    // for (int i = 0; i < nlines; i++)
    // {
    //     for (int j = 0; j < ncols; j++)
    //     {
    //         infile.read((char *)&data[i][j], sizeof(double));
    //     }
    // }
}