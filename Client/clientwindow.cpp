#include "clientwindow.h"
#include "./ui_clientwindow.h"
#include "clientwindow.h"
#include "boost/thread.hpp"

ClientWindow::ClientWindow(boost::asio::io_context& io_context, boost::asio::ip::tcp::socket&& socket, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientWindow)
    , _socket(std::move(socket))
    , _io_context(io_context)
{ ui->setupUi(this); }

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
            if(!error)
                _socket.async_read_some(boost::asio::buffer(*history_message,*message_size),
                                    [this,history_message](const boost::system::error_code& error, std::size_t)
            {
                if(!error)
                    ui->textEdit_chat->setPlainText(history_message.get()->data());
                else
                    {std::string str = error.message() + ": receive message history error.\n"; ui->statusbar->showMessage(QString::fromStdString(str),4000);}
            });
            else
                {std::string str = error.message() + ": receive message history size error.\n"; ui->statusbar->showMessage(QString::fromStdString(str),4000);}
        });
}

void ClientWindow::listen_messages()
{
    auto message_size = std::make_shared<short>();
    _socket.async_read_some(boost::asio::buffer(&message_size,sizeof(message_size)),
                            [this, message_size](const boost::system::error_code& error, std::size_t){
        if(!error)
            _socket.async_read_some(boost::asio::buffer(&_receiving_message,*message_size.get()),
                                        [this](const boost::system::error_code& error, std::size_t){
                                        ui->textEdit_chat->append(QString::fromStdString(_receiving_message));
                                        listen_messages();
                                    });
        else
            {std::string str = error.message() + ": receive message error.\n"; ui->statusbar->showMessage(QString::fromStdString(str),4000);}
    });
}

void ClientWindow::on_button_send_clicked()
{
    auto message_size = std::make_shared<short>(ui->plainTextEdit->toPlainText().size());
    _socket.async_write_some(boost::asio::buffer(message_size.get(),sizeof(message_size.get())),
            [this](const boost::system::error_code& error, std::size_t bytes_transferred)
            {
                 if(!error){
                     ui->plainTextEdit->clear();
                     _socket.async_write_some(boost::asio::buffer(ui->plainTextEdit->toPlainText()),
                            [this](const boost::system::error_code& error, std::size_t bytes_transferred)
                            {
                                if(error)
                                  ui->statusbar->showMessage(QString::fromStdString(error.message() + ": sending size of message error"),5000);
                            });
                 }else
                     ui->statusbar->showMessage(QString::fromStdString(error.message() + ": sending message error"),5000);
            });
}


void ClientWindow::on_plainTextEdit_textChanged()
{
    if (ui->plainTextEdit->toPlainText().length() > sizeof(short)) {
        QString text = ui->plainTextEdit->toPlainText();
        text = text.left(sizeof(short));
        ui->plainTextEdit->setPlainText(text);

        QTextCursor cursor = ui->plainTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->plainTextEdit->setTextCursor(cursor);
    }
}

