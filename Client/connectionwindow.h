#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QMainWindow>
#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class ConnectionWindow;
}
QT_END_NAMESPACE

class ConnectionWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ConnectionWindow(QWidget *parent = nullptr);
    ~ConnectionWindow();
private:
    Ui::ConnectionWindow *ui;
signals:
private slots:
    void on_pushButton_clicked();
};

#endif // CONNECTIONWINDOW_H
