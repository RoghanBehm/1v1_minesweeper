#include <string>
#include <iostream>
#include <asio.hpp>
#include <thread>
#include "client.hpp"
#include "../include/serialize.hpp"
#include "../include/settings.hpp"

using asio::ip::tcp;

NetworkClient::NetworkClient(::asio::io_context &ioc, const std::string &host, const std::string &port)
    : socket_(ioc)
{
    tcp::resolver resolver(ioc);
    auto endpoints = resolver.resolve(host, port);
    int retries = 10;
    bool connected = false;

    for (int i = 0; i < retries; ++i) {
        try {
            asio::connect(socket_, endpoints);
            connected = true;
            break;
        } catch (const std::system_error &e) {
            std::cerr << "Connect attempt " << (i + 1) << " failed: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

    if (!connected) {
        throw std::runtime_error("Unable to connect to server after multiple attempts.");
    }

    read_seed();
}

void NetworkClient::send_message(const std::vector<char> &message)
{

    asio::post(socket_.get_executor(), [this, message]()
                      { asio::async_write(socket_,
                            asio::buffer(message),
                            [](const std::error_code &ec, std::size_t bytes)
                            {
                                if (!ec)
                                    std::cout << "Sent " << bytes << " bytes.\n";
                                else
                                    std::cerr << "Write error: " << ec.message() << std::endl;
                            }); });
}

void NetworkClient::read_seed()
{
    auto buffer = std::make_shared<std::vector<char>>(12);

    asio::async_read(
        socket_,
        asio::buffer(*buffer),
        asio::transfer_exactly(12),
        [this, buffer](const std::error_code &ec, std::size_t bytes_transferred)
        {
            if (!ec && bytes_transferred == 12)
            {
                MessageType type;
                std::memcpy(&type, buffer->data(), sizeof(MessageType));

                if (type == MessageType::Seed)
                {
                    int seed, num_mines;
                    std::memcpy(&seed, buffer->data() + sizeof(MessageType), sizeof(seed));
                    std::memcpy(&num_mines, buffer->data() + sizeof(MessageType) + sizeof(seed), sizeof(num_mines));

                    std::cout << "Received seed: " << seed << "\n" << "Received mine number:" << num_mines << std::endl;
                    std::srand(seed);
                    config.mine_number = num_mines;
                    config.seed_received.store(true, std::memory_order_release);
                    async_read();
                }
                else
                {
                    std::cerr << "Expected seed message, got something else.\n";
                }
                
            }
            else
            {
                std::cerr << "Read error (seed): " << ec.message() << std::endl;
            }
        });
}

void NetworkClient::async_read()
{
    auto buffer = std::make_shared<std::vector<char>>(128);
    socket_.async_read_some(
        asio::buffer(*buffer),
        [this, buffer](const std::error_code& ec, std::size_t bytes_transferred)
        {
            if (!ec)
            {
                // Place incoming data in persistent buffer (avoids overwrites)
                incoming_data_.insert(incoming_data_.end(), buffer->begin(), buffer->begin() + bytes_transferred);

                // Ensure we receive data == at least the prefix length
                while (incoming_data_.size() >= sizeof(uint32_t))
                {
                    uint32_t body_size;
                    std::memcpy(&body_size, incoming_data_.data(), sizeof(uint32_t));

                    // Check for full message
                    if (incoming_data_.size() < sizeof(uint32_t) + body_size)
                    {
                        break;
                    }

                    // Extract complete message
                    std::vector<char> message(incoming_data_.begin() + sizeof(uint32_t),
                                              incoming_data_.begin() + sizeof(uint32_t) + body_size);

                    incoming_data_.erase(incoming_data_.begin(),
                                         incoming_data_.begin() + sizeof(uint32_t) + body_size);

                    // Deserialize message (without prepended body_size and MessageType)
                    try
                    {
                        MessageType type;
                        std::memcpy(&type, message.data(), sizeof(MessageType));
                        auto message_data = std::vector<char>(message.begin() + sizeof(MessageType), message.end());

                        if (type == MessageType::Coordinates)
                        {
                            coords = deserialize_pairs(message_data);
                            all_coords.insert(all_coords.end(), coords.begin(), coords.end());

                            std::cout << "Received coords: ";
                            for (const auto& [x, y] : coords)
                                std::cout << "(" << x << ", " << y << ") ";
                            std::cout << std::endl;
                        } else if (type == MessageType::Result)
                        {
                            game_result = deserialize_result(message_data);
                            std::cout << game_result << std::endl;
                        }
                        else if (type == MessageType::Seed)
                        {
                            read_seed();
                            config.regenerate = true;
                        }
                        else if (type == MessageType::Rematch) {
                            receiveRestartRequest();
                        }
                        else
                        {
                            std::cerr << "Unknown message type\n";
                        }
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << "Error deserializing message: " << e.what() << std::endl;
                    }
                }

                async_read();
            }
            else
            {
                std::cerr << "Read error: " << ec.message() << std::endl;
            }
        });
}

void NetworkClient::receiveRestartRequest()
{
    opponentRestartRequested = true;
    std::cout << "Opponent requested a restart" << std::endl;
}

bool NetworkClient::hasOpponentRestarted() {
    return opponentRestartRequested;
}

void NetworkClient::clearEnemyData() {
    all_coords.clear();
}


std::vector<std::pair<int,int>> NetworkClient::return_board()
{
    config.coords_received = false;
    return all_coords; 
}

int NetworkClient::return_res()
{
    return game_result;
}