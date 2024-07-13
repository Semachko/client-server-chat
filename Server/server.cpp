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

void Server::send_message_history(const p_socket &socket)
{
    auto message_size = std::make_shared<std::size_t>();
    auto history = std::make_shared<std::string>();
    *history = _history_message.to_string();
    *message_size = history->size();

    socket->async_write_some(boost::asio::buffer(message_size.get(), sizeof(*message_size)),
             [&socket, history, message_size](boost::system::error_code ec, std::size_t){
                 if(!ec && history.get()->size()!=0){
                     socket->write_some(boost::asio::buffer(*history,*message_size),ec);
                     if(ec)
                         std::cout<<ec.message() + ": send message history error.\n";
                 }
                 else
                     std::cout<<ec.message() + ": send message history size error.\n";
             });
}

void Server::start_reading(const std::unordered_map<std::string,p_socket>::iterator& user)
{   // Reading size of message.
    auto message = std::make_shared<Message>();

    user->second->async_read_some(boost::asio::buffer(&message->size, sizeof(message->size)),
                            [this, user, message](const boost::system::error_code& error, std::size_t bytes_transferred){
        if (!error){ // Reading message.
            user->second->async_read_some(boost::asio::buffer(message->data, message->size),
                                    [this, user, message](const boost::system::error_code& error, std::size_t bytes_transferred){
                if(!error){
                    std::string user_message = user->first + ": " + message->data;
                    _history_message.push_message(user_message);
                    send_new_message(std::move(message));
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

void Server::send_new_message(const std::shared_ptr<Message>&& message)
{
    for(auto& conns : _connections)
    {
        conns.second->async_write_some(boost::asio::buffer(&message->size, sizeof(message->size)),
                                 [&conns, message](boost::system::error_code ec, std::size_t){
           if(!ec){
                conns.second->async_write_some(boost::asio::buffer(message->data),[&conns](boost::system::error_code ec, std::size_t){
                   if(ec)
                       std::cout<<"User " + conns.first + " disconnected\n";
               });
           } else
                std::cout<<"User " + conns.first + " disconnected\n";
        });
    }
}


