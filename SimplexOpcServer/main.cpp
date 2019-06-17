#include <QCoreApplication>
#include <QDebug>

#include <QUaServer>

#include "temperaturesensor.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QUaServer server;

	QUaFolderObject * objsFolder = server.objectsFolder();

	// register new type

	server.registerType<TemperatureSensor>();

	// create new type instances

    auto sensor1 = objsFolder->addChild<TemperatureSensor>();
    sensor1->setDisplayName("Sensor1");

    QUaBaseDataVariable * varBaseData = sensor1->addBaseDataVariable("ns=1;s=MyStatus");
    varBaseData->setWriteAccess(true);
    varBaseData->setDataType(QMetaType::QString);
    varBaseData->setValue("OFF");
    varBaseData->setBrowseName("status");

//	auto sensor2 = objsFolder->addChild<TemperatureSensor>();
//	sensor2->setDisplayName("Sensor2");
//	auto sensor3 = objsFolder->addChild<TemperatureSensor>();
//	sensor3->setDisplayName("Sensor3");

	server.start();

	return a.exec(); 
}
