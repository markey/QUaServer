#include <QApplication>

#include "SimplexOpcServer.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SimplexOpcServer server;
    server.show();

	return a.exec(); 
}
