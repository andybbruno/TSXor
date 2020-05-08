#include <vector>
#include "Predictors/dfcm.cpp"
#include "Predictors/fcm.cpp"

class FPC
{
private:
public:
    size_t log_size = 16;
    FCM fcm{log_size};
    DFCM dfcm{log_size};

    size_t bytesize(int zeros)
    {
        uint64_t bytes = zeros / 8;
        return (bytes == 8) ? (size_t)7 : bytes;
    }

    std::pair<uint64_t, uint64_t> encode(double d)
    {
        uint64_t d_int = *((uint64_t *)&d);
        auto fcm_xor = fcm.getPrediction() ^ d_int;
        auto dfcm_xor = dfcm.getPrediction() ^ d_int;
        auto fcm_zeros = __builtin_clzll(fcm_xor);
        auto dfcm_zeros = __builtin_clzll(dfcm_xor);

        bool fcm_is_better = fcm_zeros > dfcm_zeros;

        fcm.update(d_int);
        dfcm.update(d_int);

        uint64_t code = 0;
        if (fcm_is_better)
        {
            code = bytesize(fcm_zeros);
            code |= 0x08;
            return std::pair(code, fcm_xor);
        }
        else
        {
            code = bytesize(dfcm_zeros);
            return std::pair(code, dfcm_xor);
        }
    }

    void head(double d)
    {
        uint64_t d_int = *((uint64_t *)&d);
        fcm.update(d_int);
        dfcm.update(d_int);
    }
};