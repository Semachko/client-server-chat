#include "server.h"

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
        if(!error)
        {
            handle_accept(*socket);
            wait_for_connections();
        }else
            std::cout<<error.message() + ": Error connecting with new user.\n";
    });
}

void Server::handle_accept(const p_socket& socket)
{   // Reading username.
    boost::asio::async_read_until(*socket, boost::asio::dynamic_buffer(_username),'\0',
                                  [this,&socket](const boost::system::error_code& error, std::size_t bytes_transferred){
        if(!error){
            auto result = _connections.insert(std::make_pair(std::move(_username), socket));
            bool username_available = result.second;

            boost::system::error_code er;
            socket->write_some(boost::asio::buffer(&username_available,1),er);

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
        }else
            std::cout<<error.message() + ": reading username error.\n";
    });
}

void Server::start_reading(const std::unordered_map<std::string,p_socket>::iterator& user)
{   // Reading size of message.
    user->second->async_read_some(boost::asio::buffer(&_message.size, sizeof(_message.size)),
                            [this, user](const boost::system::error_code& error, std::size_t bytes_transferred){
        if (!error){ // Reading message.
            user->second->async_read_some(boost::asio::buffer(_message.data, _message.size),
                                    [this, user](const boost::system::error_code& error, std::size_t bytes_transferred){
                if(!error){
                    std::string message = user->first + ": " + _message.data;
                    _history_message.push_message(message);
                    send_new_message();
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

void Server::send_message_history(const p_socket &socket)
{
    std::shared_ptr<std::string> history = std::make_shared<std::string>();
    std::shared_ptr<std::size_t> message_size = std::make_shared<std::size_t>();

    socket->async_write_some(boost::asio::buffer(&message_size, sizeof(message_size)),
       [this, &socket, history, message_size](boost::system::error_code ec, std::size_t){
            if(!ec && history.get()->size()!=0){
                socket->write_some(boost::asio::buffer(history.get(),*message_size.get()),ec);
                if(ec)
                    std::cout<<ec.message() + ": send message history error.\n";
            }
            else
                std::cout<<ec.message() + ": send message history size error.\n";
       });
}

void Server::send_new_message()
{
    for(auto& conns : _connections)
    {
        conns.second->async_write_some(boost::asio::buffer(&_message.size, sizeof(_message.size)),
                                 [this, &conns](boost::system::error_code ec, std::size_t){
           if(!ec){
               conns.second->write_some(boost::asio::buffer(_message.data));
           } else
               std::cout<<ec.message() + ": send_message error.\n";
        });
    }
}


