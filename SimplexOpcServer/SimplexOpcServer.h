#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SimplexOpcServer.h"


class SimplexOpcServer : public QMainWindow
{
	Q_OBJECT

public:
    SimplexOpcServer(QWidget *parent = Q_NULLPTR);

private:
    Ui::SimplexOpcServerClass ui;

private slots:
};
