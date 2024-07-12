#include "clientwindow.h"
#include "./ui_clientwindow.h"
#include "clientwindow.h"

ClientWindow::ClientWindow(boost::asio::ip::tcp::socket&& socket, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientWindow)
    , _socket(std::move(socket))
{
    ui->setupUi(this);
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::start()
{
    load_message_history();
}

void ClientWindow::load_message_history()
{
    std::shared_ptr<std::size_t> message_size = std::make_shared<std::size_t>();
    std::shared_ptr<std::string> history_message = std::make_shared<std::string>();
    _socket.async_read_some(boost::asio::buffer(message_size.get(),sizeof(message_size.get())),
                            [this,message_size,history_message](const boost::system::error_code& error, std::size_t)
        {
            if(!error)
                _socket.async_read_some(boost::asio::buffer(history_message.get(),*message_size.get()),
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

void ClientWindow::on_button_send_clicked()
{

}


void ClientWindow::on_plainTextEdit_textChanged()
{
    if (ui->plainTextEdit->toPlainText().length() > 11) {
        QString text = ui->plainTextEdit->toPlainText();
        text = text.left(11);
        ui->plainTextEdit->setPlainText(text);

        QTextCursor cursor = ui->plainTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->plainTextEdit->setTextCursor(cursor);
    }
}

