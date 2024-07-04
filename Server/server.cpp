#include "server.h"
#include <iostream>
using boost::asio::ip::tcp;

Server::Server(tcp ip_type, int port)
    :_io_context(),_socket(std::make_shared<tcp::socket>(_io_context)),_acceptor(_io_context)
{
    _acceptor.bind(tcp::endpoint(ip_type, port));
}

void Server::start()
{
    wait_for_connections();
    _io_context.run();
}
void Server::wait_for_connections()
{
    _acceptor.async_accept(*_socket,[this](const boost::system::error_code& error){
        handle_accept(_socket, error);
    });
}
void Server::handle_accept(p_socket& socket, const boost::system::error_code& error)
{
    if(!error)
    {

        boost::asio::async_read_until(*_socket, boost::asio::dynamic_buffer(_username),'\n',
                                      [this](const boost::system::error_code& error, std::size_t bytes_transferred){
            _connections.insert(
                std::make_pair(
                    std::move(_username), std::make_shared<tcp::socket>(_socket)));
            //async_read(_socket);
            //wait_for_connections();
        });
    }
    else
        std::cout<<error.message()<<std::endl;
}

/*
void Server::async_read(p_socket& socket)
{

    socket->async_read_some(_headerBuffer, [this, &socket](){
        socket->async_read_some(boost::asio::buffer(_message ,_headerBuffer), [this, &socket](){
            send_new_message();
            async_read(socket);
        });
    });
}

void Server::send_new_message()
{
    for(auto& [username, socket] : _connections)
    {
        socket->async_write_some(sizeof(_message), [this, &socket](){
            socket->write_some(_message);
        });
    }
}
*/
