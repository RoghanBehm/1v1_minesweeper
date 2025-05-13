#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <asio.hpp>
#include <asio/ip/host_name.hpp>
#include <asio/ip/tcp.hpp>
#include <cstdlib> 
#include "../include/serialize.hpp"


using asio::ip::tcp;

class tcp_server;

class tcp_connection : public std::enable_shared_from_this<tcp_connection>
{
public:
    using pointer = std::shared_ptr<tcp_connection>;

    static pointer create(tcp_server& server, asio::io_context& io_context)
    {
        return pointer(new tcp_connection(server, io_context));
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        do_read();
    }

    void send_message(const std::vector<char>& message)
    {
        auto self = shared_from_this();
        asio::async_write(socket_, asio::buffer(message),
            [this, self](const std::error_code& error, std::size_t /*bytes_transferred*/) {
                if (error)
                {
                    std::cout << "Error sending coordinates: " << error.message() << std::endl;
                }
            });
    }

private:
    tcp_connection(tcp_server& server, asio::io_context& io_context)
        : server_(server), socket_(io_context)
    {
    }

    void do_read();

    tcp_server& server_;
    tcp::socket socket_;
};

class tcp_server
{
public:
    tcp_server(asio::io_context& io_context, int seed, int num_mines, int port)
        : seed_(seed), num_mines_(num_mines), io_context_(io_context), acceptor_(io_context, tcp::endpoint(asio::ip::address_v4::any(), port))

    {
        print_host_ip();

        acceptor_.set_option(
            asio::socket_base::reuse_address(true));
        start_accept();
    }

    void broadcast_message(const std::vector<char>& message, std::shared_ptr<tcp_connection> sender)
    {
        for (auto& client : clients_)
        {
            if (client != sender)
            {
                client->send_message(message);
            }
        }
    }

    void add_client(std::shared_ptr<tcp_connection> client)
    {
        clients_.push_back(client);
    }

    void remove_client(std::shared_ptr<tcp_connection> client)
    {
        clients_.erase(std::remove(clients_.begin(), clients_.end(), client), clients_.end());
    }

private:
    void start_accept()
    {
        auto new_connection = tcp_connection::create(*this, io_context_);

        acceptor_.async_accept(new_connection->socket(),
            [this, new_connection](const std::error_code& error) {
                if (!error)
                {
                    std::cout << "New client connected!" << std::endl;

                    std::vector<char> serialized_seed = serialize_seed_and_mines(seed_, num_mines_);
                    new_connection->send_message(serialized_seed);

                    add_client(new_connection);
                    new_connection->start();
                }
                
                start_accept();
            });
    }


void print_host_ip()
{
    std::cout << "Fetching public IP..." << std::endl;
    system("curl -s ifconfig.me");  
    std::cout << std::endl;
}


    int seed_;
    int num_mines_;
    asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<tcp_connection>> clients_;
};

void tcp_connection::do_read()
{
    auto message = std::make_shared<std::vector<char>>(128);

    auto self = shared_from_this();
socket_.async_read_some(
    asio::buffer(*message),
    [this, self, message](const std::error_code& error, std::size_t bytes_transferred)
    {
        if (!error)
        {
            message->resize(bytes_transferred);

            auto deserialized_coords = deserialize_pairs(*message);
            if (!deserialized_coords.empty())
            {
                std::cout << "Received coordinates: ";
                for (auto& [x, y] : deserialized_coords)
                    std::cout << "(" << x << ", " << y << ") ";
                std::cout << std::endl;

                server_.broadcast_message(*message, self);
            }

            do_read(); // keep reading
        }
        else
        {
            std::cout << "Client disconnected." << std::endl;
            server_.remove_client(self);
        }
    }
);

}




int main(int argc, char** argv)
{
    int seed = std::time(nullptr);
    int num_mines = std::stoi(argv[1]);
    int port = std::stoi(argv[2]);

    std::cout << "Port:" << port << std::endl;
    try
    {
        asio::io_context io_context;

        tcp_server server(io_context, seed, num_mines, port);

        io_context.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}