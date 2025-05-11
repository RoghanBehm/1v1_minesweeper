#include <vector>
#include <cstdint>

enum class MessageType : std::int32_t { Seed, Coordinates, Result, Rematch, Mine };

std::vector<char> serialize_pairs(const std::vector<std::pair<int, int>>& coords);
std::vector<std::pair<int, int>> deserialize_pairs(const std::vector<char>& buffer);
std::vector<char> serialize_seed_and_mines(int seed, int num_mines);
int deserialize_int(const std::vector<char>& buffer);
std::vector<char> serialize_result(int result);
int deserialize_result(const std::vector<char>& buffer);
std::vector<char> serialize_bool_restart(bool b);
std::vector<char> serialize_numMines(int mines);