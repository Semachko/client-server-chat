#include "clientwindow.h"
#include "./ui_clientwindow.h"
#include "clientwindow.h"
#include "boost/thread.hpp"
#include <QDebug>
ClientWindow::ClientWindow(boost::asio::io_context& io_context, boost::asio::ip::tcp::socket&& socket, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientWindow)
    , _socket(std::move(socket))
    , _io_context(io_context)
{
    ui->setupUi(this);
    connect(this, &ClientWindow::newChatMessage, this, &ClientWindow::add_new_chatmessage);
    connect(this, &ClientWindow::settingStatusBarMessage, this, &ClientWindow::set_statusbar_message);
}

ClientWindow::~ClientWindow()
{ delete ui; }


void ClientWindow::start()
{
    load_message_history();
    listen_messages();
    boost::thread th([this](){
         _io_context.run();
    });
    th.detach();
}

void ClientWindow::load_message_history()
{
    std::shared_ptr<std::size_t> message_size = std::make_shared<std::size_t>();
    std::shared_ptr<std::string> history_message = std::make_shared<std::string>();
    _socket.async_read_some(boost::asio::buffer(message_size.get(),sizeof(*message_size)),
                            [this,message_size,history_message](const boost::system::error_code& error, std::size_t)
        {
            history_message->resize(*message_size);
            if(!error)
                _socket.async_read_some(boost::asio::buffer(*history_message,*message_size),
                                    [this,history_message](const boost::system::error_code& error, std::size_t)
                        {
                            if(!error)
                                {
                                    qDebug()<<"String = " + *history_message;
                                    emit newChatMessage(*history_message);}
                            else
                                emit settingStatusBarMessage(error.message() + ": receive message history error.\n");
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
                            [this, message_size, message_data](const boost::system::error_code& error, std::size_t){
        if(!error)
            _socket.async_read_some(boost::asio::buffer(message_data.get(),*message_size),
                                        [this,message_data](const boost::system::error_code& error, std::size_t){
                                        emit newChatMessage(*message_data);
                                        listen_messages();
                                    });
        else
            emit settingStatusBarMessage(error.message() + ": receive message error.\n");
    });
}

void ClientWindow::on_button_send_clicked()
{
    auto message_size = std::make_shared<short>(ui->plainTextEdit->toPlainText().size());
    _socket.async_write_some(boost::asio::buffer(message_size.get(),sizeof(*message_size)),
            [this](const boost::system::error_code& error, std::size_t bytes_transferred)
            {
                 if(!error){
                     ui->plainTextEdit->clear();
                     _socket.async_write_some(boost::asio::buffer(ui->plainTextEdit->toPlainText()),
                            [this](const boost::system::error_code& error, std::size_t bytes_transferred)
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
{
    //ui->textEdit_chat->append(message.get()->data());
    ui->textEdit_chat->append(message.data());
}

void ClientWindow::set_statusbar_message(const std::string& message)
{
    ui->statusbar->showMessage(message.data(),4000);
}

