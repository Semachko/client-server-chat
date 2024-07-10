#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <boost/asio.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class ClientWindow;
}
QT_END_NAMESPACE

class ClientWindow : public QWidget
{
    Q_OBJECT

public:
    ClientWindow(boost::asio::ip::tcp::socket&& socket, QWidget *parent = nullptr);
    ~ClientWindow();

    void start();
private:
    Ui::ClientWindow *ui;
    boost::asio::ip::tcp::socket _socket;
    std::string _username;
};
#endif // CLIENTWINDOW_H
