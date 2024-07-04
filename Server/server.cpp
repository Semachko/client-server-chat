#include "server.h"
#include <iostream>
using boost::asio::ip::tcp;

Server::Server(tcp ip_type, int port)
    :_io_context(),_socket(_io_context),_acceptor(_io_context)
{
    _acceptor.bind(tcp::endpoint(ip_type, port));
}

void Server::start()
{
    _acceptor.async_accept(_socket,[this](const boost::system::error_code& error){
        handle_accept(_socket, error);
    });

}

void Server::handle_accept(tcp::socket& socket, const boost::system::error_code& error)
{
    if(!error)
    {
        std::string username;
        boost::asio::async_read_until(_socket, boost::asio::dynamic_buffer(username),'\n', [this, &username](){
            _connections.insert(
                std::make_pair(
                    username, std::move(_socket)));
        });

    }
    else
        std::cout<<error.message()<<std::endl;
}
