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

}

void ClientWindow::load_message_history()
{
    _socket.async_read_some()
}
