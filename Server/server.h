#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

class Server
{
private:
    boost::asio::io_context _io_context;
    tcp::socket _socket;
    tcp::acceptor _acceptor;
    std::unordered_map<std::string,tcp::socket> _connections;
    boost::asio::streambuf _streamBuf {65536};
public:
    Server(tcp ip_type = tcp::v4(), int port=44444);
    void start();
private:
    void handle_accept(tcp::socket& socket, const boost::system::error_code& error);
};

#endif // SERVER_H
