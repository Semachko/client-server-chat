#include "connectionwindow.h"
#include "./ui_connectionwindow.h"
#include "clientwindow.h"
ConnectionWindow::ConnectionWindow(QWidget *parent)
    : QMainWindow{parent}
    , ui(new Ui::ConnectionWindow)
    , _io_context()
    , _socket(_io_context)
{
    ui->setupUi(this);
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
    _socket.close();
}

void ConnectionWindow::on_button_connect_clicked()
{
    using namespace boost::asio;
    ip::tcp::resolver resolver(_io_context);

    std::string ip = ui->lineEdit_IP->text().toStdString();
    std::string port = ui->lineEdit_port->text().toStdString();

    boost::system::error_code ec;
    auto endpoints = resolver.resolve(ip,port,ec);

    if(!ec)
    {
        boost::asio::connect(_socket, endpoints, ec);
        if(!ec)
        {
            std::string nickname = ui->lineEdit_nickname->text().toStdString() + '\0';
            if(nickname.size()>50) {
                ui->statusbar->showMessage("Error. Username can be no longer than 50 characters.", 5000);
                return;
            }
            _socket.write_some(boost::asio::buffer(nickname),ec);
            if(!ec)
            {
                char success;
                _socket.read_some(boost::asio::buffer(&success,1),ec);
                if(!ec)
                {
                    if(success!=0)
                    {
                        window = std::make_unique<ClientWindow>(_io_context,std::move(_socket));
                        window->show();
                        window->start();
                        hide();                        
                    }else
                        ui->statusbar->showMessage("This nickname is already taken.", 5000);
                }else
                    ui->statusbar->showMessage("Error reading success.", 5000);
            }else
                ui->statusbar->showMessage("Error sending username.", 5000);
        }else
            ui->statusbar->showMessage("Connection error. Maybe ip-address is wrong/server isn`t working.", 7000);
    }else
        ui->statusbar->showMessage("Wrong address",2000);
}

