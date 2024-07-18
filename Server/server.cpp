#include "server.h"
#include <iostream>

using boost::asio::ip::tcp;

Server::Server(int message_history_lenght, tcp ip_type, int port)
    :
    _io_context(),
    _acceptor(_io_context, tcp::endpoint(ip_type,port)),
    _history_message(message_history_lenght)
{
}

void Server::start()
{
    wait_for_connections();
    std::cout<<"Server started.\n";
    _io_context.run();
}
void Server::wait_for_connections()
{
    auto socket = _unhandled_sockets.emplace(std::make_shared<tcp::socket>(_io_context)).first;
    _acceptor.async_accept(**socket,[this, socket](const boost::system::error_code& error){
        if(!error) {
            handle_accept(*socket);
            wait_for_connections();
        }else
            std::cout<<error.message() + ": Error connecting with new user.\n";
    });
}

void Server::handle_accept(const p_socket& socket)
{   // Reading username.
    boost::asio::async_read_until(*socket, boost::asio::dynamic_buffer(_username),'\0',
                                  [this,socket](const boost::system::error_code& error, std::size_t bytes_transferred){
        if(!error){
            auto result = _connections.insert(std::make_pair(std::move(_username), socket));
            char username_available = result.second;

            socket->async_write_some(boost::asio::buffer(&username_available,1),
                [this,result,username_available,socket](boost::system::error_code ec, std::size_t)
                {
                    if(username_available) {
                        send_message_history(result.first->second);
                        start_reading(result.first);
                        _unhandled_sockets.erase(socket);
                        std::cout<<"User "<<result.first->first<<" connected.\n";
                    }else{
                        std::cout<<"The specified name is already taken.\n";
                        _connections.erase(result.first);
                        _unhandled_sockets.erase(socket);
                    }
                });
        }else
            std::cout<<error.message() + ": reading username error.\n";
    });
}

void Server::send_message_history(const p_socket &socket)
{
    auto history = std::make_shared<std::string>(_history_message.to_string());
    if(history->size() < 2)
        *history="\n";
    auto message_size = std::make_shared<std::size_t>(history->size());
    std::cout<<"Size of message = " << *message_size << '\n';
    socket->async_write_some(boost::asio::buffer(message_size.get(), sizeof(*message_size)),
            [&socket, history, message_size](boost::system::error_code ec, std::size_t)
            {
                if(!ec){
                    std::cout<<"Size of size message = " << *message_size << '\n';
                    std::cout<<"Message = " << *history << '\n';
                    std::cout<<"Size of real message = " << history->size() << '\n';
                    socket->async_write_some(boost::asio::buffer(history->data(),*message_size),
                        [history](boost::system::error_code ec, std::size_t)
                        {
                            if(!ec)
                                std::cout<<"Sended message history: " + *history;
                            else
                                std::cout<<ec.message() + ": send message history error.\n";
                        });
                }else
                    std::cout<<ec.message() + ": send message history size error.\n";
            });
}

void Server::start_reading(const std::unordered_map<std::string,p_socket>::iterator& user)
{   // Reading size of message.
    auto message_size = std::make_shared<short>();
    auto message_data = std::make_shared<std::string>();

    user->second->async_read_some(boost::asio::buffer(message_size.get(), sizeof(*message_size)),
        [this, user, message_size, message_data](const boost::system::error_code& error, std::size_t bytes_transferred)
        {
            if (!error){ // Reading message.
                message_data->resize(*message_size);
                user->second->async_read_some(boost::asio::buffer(*message_data, *message_size),
                                        [this, user, message_data, message_size](const boost::system::error_code& error, std::size_t bytes_transferred){
                    if(!error){
                        std::string user_message = user->first.data();
                        user_message += ": " + *message_data;
                        std::cout<<user_message<<std::endl;

                        _history_message.push_message(user_message);
                        send_new_message(user_message);
                        start_reading(user);
                    }else
                        std::cout<<error.message()+ ": reading message error.\n";
                });
            }else{ // User disconnected. Removing user.
                std::cout<<"User "+user->first+" disconnected.\n";
                _connections.erase(user);
            }
        });
}

void Server::send_new_message(const std::string user_message)
{
    auto message_size = std::make_shared<short>(static_cast<short>(user_message.size()));
    auto message = std::make_shared<std::string>(user_message);

    for(auto& conns : _connections)
    {
        conns.second->async_write_some(boost::asio::buffer(message_size.get(), sizeof(*message_size)),
            [&conns, message,message_size](boost::system::error_code ec, std::size_t){
            if(!ec)
                conns.second->async_write_some(boost::asio::buffer(message->data(), *message_size),
                    [&conns](boost::system::error_code ec, std::size_t)
                    {
                        if(ec)
                           std::cout<<"User " + conns.first + " disconnected\n";
                    });
            else
                std::cout<<"User " + conns.first + " disconnected\n";
        });
    }
}


