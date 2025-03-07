#pragma once
#include <boost/asio.hpp>

class NetworkClient
{
public:
    NetworkClient(boost::asio::io_context& ioc, const std::string& host, const std::string& port);
    void send_message(const std::vector<char>& message);
    void read_seed();
    std::vector<std::pair<int,int>> return_board();
    int return_res();
    int seed;
    void receiveRestartRequest();
    bool hasOpponentRestarted();
    void clearEnemyData();
private:
    void async_read();
    boost::asio::ip::tcp::socket socket_;
    std::vector<std::pair<int, int>> coords;
    std::vector<std::pair<int, int>> all_coords;
    std::vector<char> incoming_data_;
    bool game_result;
    bool opponentRestartRequested = false;
    
};