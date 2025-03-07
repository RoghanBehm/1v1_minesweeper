#include <vector>
#include <cstdint>

enum class MessageType : std::int32_t { Seed, Coordinates, Result, Rematch };

std::vector<char> serialize_pairs(const std::vector<std::pair<int, int>>& coords);
std::vector<std::pair<int, int>> deserialize_pairs(const std::vector<char>& buffer);
std::vector<char> serialize_seed(int seed);
int deserialize_seed(const std::vector<char>& buffer);
std::vector<char> serialize_result(int result);
int deserialize_result(const std::vector<char>& buffer);
std::vector<char> serialize_bool_restart(bool b);
