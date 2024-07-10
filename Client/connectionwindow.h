#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <boost/asio.hpp>
#include <memory>
#include "clientwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ConnectionWindow;
}
QT_END_NAMESPACE

class ConnectionWindow : public QMainWindow
{
    Q_OBJECT
public:
    ConnectionWindow(QWidget *parent = nullptr);
    ~ConnectionWindow();

private:
    Ui::ConnectionWindow *ui;
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket;
    std::unique_ptr<ClientWindow> window;

signals:
private slots:
    void on_button_connect_clicked();
};

#endif // CONNECTIONWINDOW_H
