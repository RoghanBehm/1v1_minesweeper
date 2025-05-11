#include <cstring>
#include "serialize.hpp"




std::vector<char> serialize_pairs(const std::vector<std::pair<int, int>>& coords)
{
    // Calculate sizes
    uint32_t body_size = sizeof(MessageType) + coords.size() * 2 * sizeof(int);
    uint32_t total_size = sizeof(uint32_t) + body_size;

    std::vector<char> buffer(total_size);
    char* ptr = buffer.data();

    //Prefix length
    std::memcpy(ptr, &body_size, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
    
    MessageType type = MessageType::Coordinates;
    std::memcpy(ptr, &type, sizeof(MessageType));
    ptr += sizeof(MessageType);

    for (const auto& [x, y] : coords)
    {
        std::memcpy(ptr, &x, sizeof(int));
        ptr += sizeof(int);
        std::memcpy(ptr, &y, sizeof(int));
        ptr += sizeof(int);
    }

    return buffer;
}


std::vector<std::pair<int, int>> deserialize_pairs(const std::vector<char>& buffer)
{
    std::vector<std::pair<int, int>> coords(buffer.size() / (2 * sizeof(int)));
    const char* ptr = buffer.data();

    for (auto& [x, y] : coords)
    {
        std::memcpy(&x, ptr, sizeof(int));
        ptr += sizeof(int);
        std::memcpy(&y, ptr, sizeof(int));
        ptr += sizeof(int);
    }

    return coords;
}

std::vector<char> serialize_result(int result)
{
    uint32_t body_size = sizeof(MessageType) + sizeof(uint8_t);
    uint32_t total_size = sizeof(uint32_t) + body_size;

    std::vector<char> buffer(total_size);
    char* ptr = buffer.data();

    std::memcpy(ptr, &body_size, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    MessageType type = MessageType::Result;
    std::memcpy(ptr, &type, sizeof(MessageType));
    ptr += sizeof(MessageType);

    uint8_t result_byte = static_cast<uint8_t>(result);
    std::memcpy(ptr, &result_byte, sizeof(uint8_t));

    return buffer;
}

int deserialize_result(const std::vector<char>& buffer)
{
    const char* ptr = buffer.data();
    uint8_t result_byte;
    std::memcpy(&result_byte, ptr, sizeof(uint8_t));
    return static_cast<int>(result_byte);
}



std::vector<char> serialize_seed_and_mines(int seed, int num_mines)
{
    std::vector<char> buffer(sizeof(MessageType) + sizeof(int) * 2);
    char* ptr = buffer.data();

    MessageType type = MessageType::Seed;
    std::memcpy(ptr, &type, sizeof(MessageType));
    ptr += sizeof(MessageType);

    std::memcpy(ptr, &seed, sizeof(int));
    ptr += sizeof(int);

    std::memcpy(ptr, &num_mines, sizeof(int));

    return buffer;
}


int deserialize_int(const std::vector<char>& buffer)
{
    int seed;
    std::memcpy(&seed, buffer.data(), sizeof(int));
    return seed;
}

std::vector<char> serialize_bool_restart(bool b)
{
    uint32_t body_size = sizeof(MessageType) + sizeof(uint8_t);

    uint32_t total_size = sizeof(uint32_t) + body_size;

    std::vector<char> buffer(total_size);
    char* ptr = buffer.data();

    std::memcpy(ptr, &body_size, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    MessageType type = MessageType::Rematch;
    std::memcpy(ptr, &type, sizeof(type));
    ptr += sizeof(type);

    uint8_t bool_byte = b ? 1 : 0;
    std::memcpy(ptr, &bool_byte, sizeof(bool_byte));

    return buffer;
}

std::vector<char> serialize_mines(int mines)
{
    std::vector<char> buffer(sizeof(MessageType) + sizeof(int));
    char* ptr = buffer.data();

    MessageType type = MessageType::Mine;
    std::memcpy(ptr, &type, sizeof(MessageType));
    ptr += sizeof(MessageType);

    std::memcpy(ptr, &mines, sizeof(int));
    return buffer;
}