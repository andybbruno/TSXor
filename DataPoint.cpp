#pragma once
#include <vector>
#include <iostream>

struct DataPoint
{
    static const int num_cols;
    const uint64_t timestamp;
    const std::vector<double> values;
    std::vector<uint64_t> xorWithPrev;

    DataPoint(uint64_t time, std::vector<double> val) : timestamp(time), values(val)
    {
        if (values.size() != num_cols)
            throw "Number of columns error!";

        xorWithPrev = std::vector<uint64_t>(values.size());
    };
};