#include "SimplexOpcServer.h"

#include "temperaturesensor.h"

SimplexOpcServer::SimplexOpcServer(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);

    m_server = new QUaServer();

    QUaFolderObject * objsFolder = m_server->objectsFolder();

    // register new type

    m_server->registerType<TemperatureSensor>();

    // create new type instances

    QUaBaseObject * sensor = nullptr;

    for ( int si = 0; si < 1000; si++ ) {
        sensor = objsFolder->addChild<TemperatureSensor>();
        sensor->setDisplayName(QString("Sensor%1").arg(si));

        for ( int i = 0; i < 10; i++ ) {
            auto varBaseData = sensor->addBaseDataVariable(QString("ns=1;s=%1_StringVar%2").arg(sensor->displayName()).arg(i));
            varBaseData->setWriteAccess(true);
            varBaseData->setDataType(QMetaType::QString);
            varBaseData->setValue("OFF");
            varBaseData->setBrowseName("status");
        }
    }

    connect(ui.startServerButton, &QPushButton::clicked, this, &SimplexOpcServer::startServer);
}

void SimplexOpcServer::startServer()
{
    m_server->start();
}

void SimplexOpcServer::stopServer()
{
}
