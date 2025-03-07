
#include <array>
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include "client.hpp"
#include "../include/serialize.hpp"
#include "../include/settings.hpp"

using boost::asio::ip::tcp;

NetworkClient::NetworkClient(boost::asio::io_context &ioc, const std::string &host, const std::string &port)
    : socket_(ioc)
{
    tcp::resolver resolver(ioc);
    auto endpoints = resolver.resolve(host, port);
    boost::asio::connect(socket_, endpoints);

    read_seed();
}

void NetworkClient::send_message(const std::vector<char> &message)
{

    boost::asio::post(socket_.get_executor(), [this, message]()
                      { boost::asio::async_write(socket_,
                            boost::asio::buffer(message),
                            [](const boost::system::error_code &ec, std::size_t bytes)
                            {
                                if (!ec)
                                    std::cout << "Sent " << bytes << " bytes.\n";
                                else
                                    std::cerr << "Write error: " << ec.message() << std::endl;
                            }); });
}

void NetworkClient::read_seed()
{
    auto buffer = std::make_shared<std::vector<char>>(8);

    boost::asio::async_read(
        socket_,
        boost::asio::buffer(*buffer),
        boost::asio::transfer_exactly(8),
        [this, buffer](const boost::system::error_code &ec, std::size_t bytes_transferred)
        {
            if (!ec && bytes_transferred == 8)
            {
                MessageType type;
                std::memcpy(&type, buffer->data(), sizeof(MessageType));

                if (type == MessageType::Seed)
                {
                    std::memcpy(&seed,
                                buffer->data() + sizeof(MessageType),
                                sizeof(seed));

                    std::cout << "Received seed: " << seed << std::endl;
                    std::srand(seed);
                }
                else
                {
                    std::cerr << "Expected seed message, got something else.\n";
                }

                if (!ec && bytes_transferred == 8)
                {
                    globalSettings.seed_received = true;
                    async_read();
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
        boost::asio::buffer(*buffer),
        [this, buffer](const boost::system::error_code& ec, std::size_t bytes_transferred)
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
                            game_result = deserialize_bool(message_data);
                        }
                        else if (type == MessageType::Seed)
                        {
                            read_seed();
                            globalSettings.regenerate = true;
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
    globalSettings.coords_received = false;
    return all_coords; 
}

bool NetworkClient::return_res()
{
    return game_result;
}
