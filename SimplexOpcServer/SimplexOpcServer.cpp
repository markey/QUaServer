#include "SimplexOpcServer.h"

#include "temperaturesensor.h"

SimplexOpcServer::SimplexOpcServer(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);

    m_server = new QUaServer();

    connect(ui.startServerButton, &QPushButton::clicked, this, &SimplexOpcServer::startServer);
    connect(ui.stopServerButton, &QPushButton::clicked, this, &SimplexOpcServer::stopServer);
}

void SimplexOpcServer::startServer()
{
    QUaFolderObject * objsFolder = m_server->objectsFolder();

    // register new type

    m_server->registerType<TemperatureSensor>();

    // create new type instances

    const auto numNestedNodes = ui.numNodes->value();
    const auto numVariables = ui.numVariables->value();

    QUaBaseObject * sensor = nullptr;

    for ( int si = 0; si < numNestedNodes; si++ ) {
        sensor = objsFolder->addChild<TemperatureSensor>();
        sensor->setDisplayName(QString("Sensor%1").arg(si));

        for ( int i = 0; i < numVariables; i++ ) {
            auto varBaseData = sensor->addBaseDataVariable(QString("ns=1;s=%1_StringVar%2").arg(sensor->displayName()).arg(i));
            varBaseData->setWriteAccess(true);
            varBaseData->setDataType(QMetaType::QString);
            varBaseData->setValue("OFF");
            varBaseData->setBrowseName("status");
        }
    }

    m_server->start();

    ui.startServerButton->setEnabled(false);
    ui.stopServerButton->setEnabled(true);
}

void SimplexOpcServer::stopServer()
{
    m_server->stop();

    ui.startServerButton->setEnabled(true);
    ui.stopServerButton->setEnabled(false);
}
