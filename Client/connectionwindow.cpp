#include "connectionwindow.h"
#include "./ui_connectionwindow.h"
#include <boost/asio.hpp>
#include <iostream>
ConnectionWindow::ConnectionWindow(QWidget *parent)
    : QMainWindow{parent},
    ui(new Ui::ConnectionWindow)
{
    std::cout<<"initiated";
    ui->setupUi(this);
}

ConnectionWindow::~ConnectionWindow()
{
     delete ui;
}

void ConnectionWindow::on_pushButton_clicked()
{

    using namespace boost::asio;

    io_context io_context;
    ip::tcp::resolver resolver(io_context);

    std::string ip = ui->lineEdit_IP->text().toStdString();
    std::string port = ui->lineEdit_port->text().toStdString();

    boost::system::error_code ec;
    auto endpoints = resolver.resolve(ip,port,ec);
    std::cout<<"works";
    if(!ec)
    {

    }
else
    ui->statusbar->showMessage("Wrong address",2);
}

