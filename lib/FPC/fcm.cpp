#include <vector>

namespace predictors
{
struct FCM
{
    std::vector<uint64_t> table;
    uint64_t fcm_hash;
    size_t table_size;

    FCM(size_t log_size)
    {
        table_size = 1 << log_size;
        table = std::vector<uint64_t>(table_size);
    }

    long getPrediction()
    {
        return table[fcm_hash];
    }

    void update(long true_value)
    {
        table[fcm_hash] = true_value;
        fcm_hash = (((fcm_hash << 6) ^ (true_value >> 48)) & (table_size - 1));
    }
};
}