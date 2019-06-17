#pragma once

#include <QtWidgets/QMainWindow>
#include <QUaServer>

#include "ui_SimplexOpcServer.h"


class SimplexOpcServer : public QMainWindow
{
	Q_OBJECT

public:
    SimplexOpcServer(QWidget *parent = Q_NULLPTR);
    ~SimplexOpcServer();

private slots:
    void startServer();
    void stopServer();

private:
    Ui::SimplexOpcServerClass ui;

    QUaServer* m_server;
};

class MyDataType : public QUaBaseDataVariable
{
    Q_OBJECT

};

