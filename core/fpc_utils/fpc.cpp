#pragma once
#include <vector>
#include <cassert>
#include "Predictors/dfcm.cpp"
#include "Predictors/fcm.cpp"

struct FPC
{
    size_t log_size = 16;
    FCM fcm{log_size};
    DFCM dfcm{log_size};

    uint64_t bytesize(uint64_t zeros)
    {
        int bytes = zeros / 8;

        // std::cout << zeros << ">>>" << bytes << std::endl;
        return (bytes == 8) ? (size_t)7 : bytes;
    }

    void head(double d)
    {
        uint64_t d_int = *((uint64_t *)&d);
        fcm.update(d_int);
        dfcm.update(d_int);
    }

    std::pair<uint64_t, uint64_t> encode(double d)
    {
        uint64_t d_int = *((uint64_t *)&d);
        uint64_t fcm_xor = fcm.getPrediction() ^ d_int;
        uint64_t dfcm_xor = dfcm.getPrediction() ^ d_int;

        uint64_t fcm_zeros = 0;
        uint64_t dfcm_zeros = 0;

        if (fcm_xor != 0)
        {
            fcm_zeros = __builtin_clzll(fcm_xor);
        }
        if (dfcm_zeros != 0)
        {
            dfcm_zeros = __builtin_clzll(dfcm_xor);
        }

        bool fcm_is_better = (fcm_zeros > dfcm_zeros);

        fcm.update(d_int);
        dfcm.update(d_int);

        uint64_t code = 0;
        if (fcm_is_better)
        {
            if (fcm_zeros > 64)
            {
                fcm_zeros = 0;
            }
            code = bytesize(fcm_zeros);
            code |= 0x08;
            // std::cout << bytesize(fcm_zeros) << " --- " << code << " --- " << fcm_xor << std::endl;
            return std::pair<uint64_t, uint64_t>(code, fcm_xor);
        }
        else
        {
            if (dfcm_zeros > 64)
            {
                dfcm_zeros = 0;
            }

            code = bytesize(dfcm_zeros);
            // std::cout << bytesize(dfcm_zeros) << " -#- " << code << " -#- " << dfcm_zeros << std::endl;
            return std::pair<uint64_t, uint64_t>(code, dfcm_xor);
        }
    }

    double decode(bool usefcm, uint64_t body)
    {
        uint64_t pred;

        // 1xxx
        if (usefcm)
        {
            pred = fcm.getPrediction();
        }
        // 0xxx
        else
        {
            pred = dfcm.getPrediction();
        }
        auto actual = pred ^ body;
        fcm.update(actual);
        dfcm.update(actual);

        double p = *((double *)&actual);
        return p;
    }
};