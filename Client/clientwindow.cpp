#include "clientwindow.h"
#include "./ui_clientwindow.h"
#include "clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);
}

ClientWindow::~ClientWindow()
{
    delete ui;
}
