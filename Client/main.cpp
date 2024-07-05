#include "connectionwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConnectionWindow m;
    m.show();
    return a.exec();
}
