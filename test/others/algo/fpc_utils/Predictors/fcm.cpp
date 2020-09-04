#include <vector>

struct FCM
{
    std::vector<uint64_t> table;
    uint64_t fcm_hash;
    size_t table_size;

    FCM(size_t log_size)
    {
        table_size = 1 << log_size;
        fcm_hash = 0;
        table = std::vector<uint64_t>(table_size, 0);
    }

    uint64_t getPrediction()
    {
        return table[fcm_hash];
    }

    void update(uint64_t true_value)
    {
        table[fcm_hash] = true_value;
        fcm_hash = (((fcm_hash << 6) ^ (true_value >> 48)) & (table_size - 1));
    }
};
