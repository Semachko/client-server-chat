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
{Q_OBJECT

private:
    Ui::ClientWindow *ui;
    boost::asio::ip::tcp::socket _socket;

public:
    ClientWindow(boost::asio::ip::tcp::socket&& socket, QWidget *parent = nullptr);
    ~ClientWindow();
    void start();

private:
    void load_message_history();

};
#endif // CLIENTWINDOW_H
