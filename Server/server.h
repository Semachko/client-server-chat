#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <boost/asio.hpp>
#include "messagehistory.h"

using boost::asio::ip::tcp;
using p_socket = std::shared_ptr<tcp::socket>;

class Server
{
    struct Message {
        short size;
        std::string data;
    };

private:
    boost::asio::io_context _io_context;
    tcp::acceptor _acceptor;
    std::string _username;
    std::unordered_set<p_socket> _unhandled_sockets;
    std::unordered_map<std::string,p_socket> _connections;
    MessageHistory _history_message;
public:
    Server(int message_history_lenght = 200, tcp ip_type = tcp::v4(), int port=44445);
    void start();
private:
    void wait_for_connections();
    void handle_accept(const p_socket& socket);
    void start_reading(const std::unordered_map<std::string,p_socket>::iterator& user);
    void send_message_history(const p_socket& socket);
    void send_new_message(const std::shared_ptr<Message>&& message);
};

#endif // SERVER_H
