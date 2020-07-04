#include <vector>
#include <cmath>
#include <string>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>

class CSVReader
{
    std::string filename;
    std::string delimeter;
    std::ifstream inFile;
    std::vector<std::string> col_names;
    std::string line;

public:
    CSVReader(std::string f, std::string del = ",") : filename(f), delimeter(del)
    {
        inFile.open(filename);
        if (!inFile)
        {
            std::cout << "Unable to open file";
            exit(1); // terminate with error
        }

        if (getline(inFile, line))
        {
            boost::algorithm::split(col_names, line, boost::is_any_of(delimeter));
        }
    };

    bool isEmpty()
    {
        getline(inFile, line);
        return ((line == "") || (!inFile));
    }

    std::vector<double> nextLine()
    {
        if (line != "")
        {
            std::vector<std::string> s;
            boost::algorithm::split(s, line, boost::is_any_of(delimeter));
            std::vector<double> vec(s.size());
            std::transform(s.begin(), s.end(), vec.begin(), [](const std::string &val) {
                return (val != "") ? double(std::stod(val)) : nan("0");
            });

            return vec;
        }
        else
        {
            inFile.close();
            exit(EXIT_FAILURE);
        }
    }

    std::vector<std::string> nextLineString()
    {
        if (getline(inFile, line))
        {
            std::vector<std::string> s;
            boost::algorithm::split(s, line, boost::is_any_of(delimeter));
            return s;
        }
        else
        {
            inFile.close();
            exit(EXIT_FAILURE);
        }
    }

    ~CSVReader()
    {
        inFile.close();
    }
};