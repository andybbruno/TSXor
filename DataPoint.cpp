#pragma once
#include <vector>

class DataPoint
{
public:
    static const int num_cols;
    const uint64_t timestamp;
    const std::vector<double> values;

    DataPoint(uint64_t time, std::vector<double> val) : timestamp(time), values(val)
    {
        if (values.size() != num_cols)
            throw "Number of columns error!";
    };
};