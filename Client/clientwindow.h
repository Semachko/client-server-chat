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
    std::string _message;

public:
    ClientWindow(boost::asio::io_context& io_context, boost::asio::ip::tcp::socket&& socket, QWidget *parent = nullptr);
    ~ClientWindow(){ delete ui; };
    void start();

private:
    void load_message_history();
    void listen_messages();

signals:
    void clearPlainTextEdit();
    void newChatMessage(const std::string& message);
    void settingStatusBarMessage(const std::string& message);

private slots:
    void on_button_send_clicked();
    void on_plainTextEdit_textChanged();
    void add_new_chatmessage(const std::string& message);
    void set_statusbar_message(const std::string& message);
};
#endif // CLIENTWINDOW_H
