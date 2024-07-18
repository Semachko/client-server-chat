#include "clientwindow.h"
#include "./ui_clientwindow.h"
#include "boost/thread.hpp"
#include <QDebug>
ClientWindow::ClientWindow(boost::asio::io_context& io_context, boost::asio::ip::tcp::socket&& socket, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientWindow)
    , _socket(std::move(socket))
    , _io_context(io_context)
{
    ui->setupUi(this);

    connect(this, &ClientWindow::clearPlainTextEdit, this, [this](){ui->plainTextEdit->clear();});
    connect(this, &ClientWindow::newChatMessage, this, [this](const std::string& message){add_new_chatmessage(message);});
    connect(this, &ClientWindow::settingStatusBarMessage, this, [this](const std::string& message){set_statusbar_message(message);});
}



void ClientWindow::start()
{
    load_message_history();
    boost::thread th([this](){
         _io_context.run();
    });
    th.detach();
}

void ClientWindow::load_message_history()
{
    auto message_size = std::make_shared<std::size_t>();
    auto history_message = std::make_shared<std::string>();
    _socket.async_read_some(boost::asio::buffer(message_size.get(),sizeof(*message_size)),
        [this,message_size,history_message](const boost::system::error_code& error, std::size_t)
        {
            qDebug()<<"Resizing history message to: " << *message_size << '\n';
            history_message->resize(*message_size);
            if(!error)
                _socket.async_read_some(boost::asio::buffer(history_message->data(),*message_size),
                        [this,history_message](const boost::system::error_code& error, std::size_t)
                        {
                            qDebug()<<"history message is: " << *history_message << '\n';
                            if(!error)
                                emit newChatMessage(*history_message);
                            else
                                emit settingStatusBarMessage(error.message() + ": receive message history error.\n");

                            listen_messages();
                        });
            else
                emit settingStatusBarMessage(error.message() + ": receive message history size error.\n");
        });
}

void ClientWindow::listen_messages()
{
    auto message_size = std::make_shared<short>();
    auto message_data = std::make_shared<std::string>();
    _socket.async_read_some(boost::asio::buffer(message_size.get(),sizeof(*message_size)),
        [this, message_size, message_data](const boost::system::error_code& error, std::size_t)
        {
            if(!error){
                message_data->resize(*message_size);
                _socket.async_read_some(boost::asio::buffer(message_data->data(),*message_size),
                        [this,message_data,message_size](const boost::system::error_code& error, std::size_t)
                        {
                            emit newChatMessage(*message_data);
                            listen_messages();
                        });
            }else
                emit settingStatusBarMessage(error.message() + ": receive message error.\n");
        });
}

void ClientWindow::on_button_send_clicked()
{
    auto message = std::make_shared<std::string>(ui->plainTextEdit->toPlainText().toStdString());
    auto message_size = std::make_shared<short>(static_cast<short>(message->size()));
    if(*message_size==0)
        return;

    _socket.async_write_some(boost::asio::buffer(message_size.get(),sizeof(*message_size)),
            [this,message_size,message](const boost::system::error_code& error, std::size_t)
            {
                if(!error){
                    emit clearPlainTextEdit();
                    qDebug()<<*message_size;
                    qDebug()<<*message;
                    _socket.async_write_some(boost::asio::buffer(message->data(),*message_size),
                        [this,message_size,message](const boost::system::error_code& error, std::size_t)
                        {
                            if(error)
                                emit settingStatusBarMessage(error.message() + ": sending size of message error\n");
                        });
                 }else
                      emit settingStatusBarMessage(error.message() + ": sending message error\n");
            });
}

void ClientWindow::on_plainTextEdit_textChanged()
{
    if (ui->plainTextEdit->toPlainText().length() > MAXSHORT) {
        QString text = ui->plainTextEdit->toPlainText();
        text = text.left(MAXSHORT);
        ui->plainTextEdit->setPlainText(text);

        QTextCursor cursor = ui->plainTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->plainTextEdit->setTextCursor(cursor);
    }
}

void ClientWindow::add_new_chatmessage(const std::string& message)
{   ui->textEdit_chat->append(message.data()); }

void ClientWindow::set_statusbar_message(const std::string& message)
{
    ui->statusbar->showMessage(message.data(),4000);
}

