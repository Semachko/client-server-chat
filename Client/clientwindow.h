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
    boost::asio::io_context& _io_context;
    std::string _message_history;
    std::string _receiving_message;
public:
    ClientWindow(boost::asio::io_context& io_context, boost::asio::ip::tcp::socket&& socket, QWidget *parent = nullptr);
    ~ClientWindow();
    void start();

private slots:
    void on_button_send_clicked();
    void on_plainTextEdit_textChanged();
private:
    void load_message_history();
    void listen_messages();
};
#endif // CLIENTWINDOW_H
