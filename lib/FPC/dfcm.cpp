#include <vector>

namespace predictors
{
struct DFCM
{
    std::vector<uint64_t> table;
    uint64_t dfcm_hash;
    uint64_t lastValue;
    size_t table_size;

    DFCM(size_t log_size)
    {
        table_size = 1 << log_size;
        table = std::vector<uint64_t>(table_size);
    }

    uint64_t getPrediction()
    {
        return table[dfcm_hash] + lastValue;
    }

    void update(uint64_t true_value)
    {
        table[dfcm_hash] = true_value - lastValue;
        dfcm_hash = (((dfcm_hash << 2) ^ ((true_value - lastValue) >> 40)) & (table_size - 1));
        lastValue = true_value;
    }
};
} // namespace predictors