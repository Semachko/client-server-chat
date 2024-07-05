#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using p_socket = std::shared_ptr<tcp::socket>;

class Server
{
private:
    boost::asio::io_context _io_context;
    std::shared_ptr<tcp::socket> _socket;
    tcp::acceptor _acceptor;
    std::unordered_map<std::string, p_socket> _connections;
    std::string _username;
    short _headerBuffer;
    std::string _message;
public:
    Server(tcp ip_type = tcp::v4(), int port=44444);
    void start();
private:
    void wait_for_connections();
    void handle_accept(p_socket& socket, const boost::system::error_code& error);
    void async_read(p_socket& socket);
    void send_new_message();
};

#endif // SERVER_H
