#include "SimplexOpcServer.h"

#include "customvar.h"
#include "temperaturesensor.h"

SimplexOpcServer::SimplexOpcServer(QWidget *parent)
    : QMainWindow(parent)
    , m_server(nullptr)
{
	ui.setupUi(this);

    connect(ui.startServerButton, &QPushButton::clicked, this, &SimplexOpcServer::startServer);
    connect(ui.stopServerButton, &QPushButton::clicked, this, &SimplexOpcServer::stopServer);
}

SimplexOpcServer::~SimplexOpcServer()
{
    delete m_server;
}


void SimplexOpcServer::startServer()
{
    m_server = new QUaServer(4841);
    QUaFolderObject * objsFolder = m_server->objectsFolder();

    /* Make your custom datatype known to the stack */
    UA_DataType *types = (UA_DataType*)UA_malloc(sizeof(UA_DataType));
    UA_DataTypeMember *members = (UA_DataTypeMember*)UA_malloc(sizeof(UA_DataTypeMember) * 3);
    members[0] = Point_members[0];
    members[1] = Point_members[1];
    members[2] = Point_members[2];
    types[0] = PointType;
    types[0].members = members;

    m_server->setCustomDataTypes(types);
    ///////////////

   UA_NodeId DataTypeEncodingNodeId;
   UA_StatusCode ret;

   UA_DataTypeAttributes dtAttr;
   UA_DataTypeAttributes_init(&dtAttr);
   dtAttr.description = UA_LOCALIZEDTEXT((char *) "en_US", (char *) "PointDataType description");
   dtAttr.displayName = UA_LOCALIZEDTEXT((char *) "en_US", (char *) "PointDataType");

   ret = UA_Server_addDataTypeNode(m_server->ua_server(), PointType.typeId,
      UA_NODEID_NUMERIC(0, UA_NS0ID_STRUCTURE),
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
      UA_QUALIFIEDNAME(1, (char *) "PointDataType"),
      dtAttr,
      NULL,
      NULL);
    Q_ASSERT(ret == UA_STATUSCODE_GOOD);



   /* Add DataTypeDescription variable */
   UA_NodeId niDataTypeDesc;
   UA_VariableAttributes vaDescription;
   UA_VariableAttributes_init(&vaDescription);
   vaDescription.displayName = UA_LOCALIZEDTEXT((char *) "en_US", (char *) "PointDataType");
   vaDescription.description = UA_LOCALIZEDTEXT((char *) "en_US", (char *) "PointDataType");
   vaDescription.valueRank = -1;
   vaDescription.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
   vaDescription.accessLevel = UA_ACCESSLEVELMASK_READ;

   UA_String bsAttrDesc = UA_STRING((char *) "PointDataType");
   UA_Variant_setScalar(&vaDescription.value, &bsAttrDesc, &UA_TYPES[UA_TYPES_STRING]);

   ret = UA_Server_addVariableNode(m_server->ua_server(),
      UA_NODEID_NULL,
      UA_NODEID_NUMERIC(0, UA_NS0ID_OPCUA_BINARYSCHEMA),
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
      UA_QUALIFIEDNAME(0, (char *) "PointDataType"),
      UA_NODEID_NUMERIC(0, UA_NS0ID_DATATYPEDESCRIPTIONTYPE),
      vaDescription, NULL, &niDataTypeDesc);
    Q_ASSERT(ret == UA_STATUSCODE_GOOD);


   /* Add DataTypeEncoding object Default Binary */
   UA_ObjectAttributes oAttr;
   UA_ObjectAttributes_init(&oAttr);
   oAttr.displayName = UA_LOCALIZEDTEXT((char * ) "en_US", (char * ) "Default Binary");
   oAttr.description = UA_LOCALIZEDTEXT((char * ) "en_US", (char * ) "Default Binary");

   ret = UA_Server_addObjectNode(m_server->ua_server(),
      UA_NODEID_NULL,
      UA_NODEID_NULL,
      UA_NODEID_NULL,
      UA_QUALIFIEDNAME(0, (char * ) "Default Binary"),
      UA_NODEID_NUMERIC(0, UA_NS0ID_DATATYPEENCODINGTYPE),
      oAttr, NULL, &DataTypeEncodingNodeId);
    Q_ASSERT(ret == UA_STATUSCODE_GOOD);


   ret = UA_Server_addReference(m_server->ua_server(),
      PointType.typeId,
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASENCODING),
      UA_EXPANDEDNODEID_NUMERIC(DataTypeEncodingNodeId.namespaceIndex, DataTypeEncodingNodeId.identifier.numeric),
      TRUE);
    Q_ASSERT(ret == UA_STATUSCODE_GOOD);

   ret = UA_Server_addReference(m_server->ua_server(),
      DataTypeEncodingNodeId,
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASDESCRIPTION),
      UA_EXPANDEDNODEID_NUMERIC(niDataTypeDesc.namespaceIndex, niDataTypeDesc.identifier.numeric),
      TRUE);
    Q_ASSERT(ret == UA_STATUSCODE_GOOD);


    // Register 3D Point
    UA_VariableTypeAttributes dattr = UA_VariableTypeAttributes_default;
    dattr.description = UA_LOCALIZEDTEXT((char *) "en-US", (char *) "3D Point");
    dattr.displayName = UA_LOCALIZEDTEXT((char *) "en-US", (char *) "3D Point");
    dattr.dataType = PointType.typeId;
    dattr.valueRank = UA_VALUERANK_SCALAR;

    Point p;
    p.x = 0.0;
    p.y = 0.0;
    p.z = 0.0;
    UA_Variant_setScalar(&dattr.value, &p, &PointType);

    ret = UA_Server_addVariableTypeNode(m_server->ua_server(), UA_NODEID_STRING(1, (char *) "3D.Point Type"),
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                  UA_QUALIFIEDNAME(1, (char *) "3D.Point"),
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                  dattr, NULL, NULL);

    Q_ASSERT(ret == UA_STATUSCODE_GOOD);


    // Instance 3D Point
    Point p1;
    p1.x = 3.0;
    p1.y = 4.0;
    p1.z = 5.0;
    UA_VariableAttributes vattr = UA_VariableAttributes_default;
    vattr.description = UA_LOCALIZEDTEXT((char *) "en-US", (char *) "3D Point");
    vattr.displayName = UA_LOCALIZEDTEXT((char *) "en-US", (char *) "3D Point");
    vattr.dataType = PointType.typeId;
    vattr.valueRank = -1;
    vattr.accessLevel = UA_ACCESSLEVELMASK_READ;
    UA_Variant_setScalarCopy(&vattr.value, &p1, &PointType);

    ret = UA_Server_addVariableNode(m_server->ua_server(), UA_NODEID_STRING(1, (char *) "3D.Point"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                              UA_QUALIFIEDNAME(1, (char *) "3D.Point"),
                              UA_NODEID_STRING(1, (char *) "3D.Point Type"),
                              vattr, NULL, NULL);

    Q_ASSERT(ret == UA_STATUSCODE_GOOD);

    // Custom Variable

    m_server->registerType<CustomVar>();

    auto custom1 = objsFolder->addChild<CustomVar>();
    custom1->setDisplayName("custom1");

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

    delete m_server;
    m_server = nullptr;

    ui.startServerButton->setEnabled(true);
    ui.stopServerButton->setEnabled(false);
}
