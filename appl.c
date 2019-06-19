#include <winsock2.h>
#include <Ws2tcpip.h>
#include <signal.h>
#include <stdio.h>

#include "ua_server.h"
#include "ua_log.h"
#include "ua_log_stdout.h"
#include "ua_config_standard.h"
#include "ua_network_tcp.h"

#pragma comment(lib, "Ws2_32.lib") 

static UA_Int32 myInteger;
static UA_NodeId niDataTypeDesc;
static UA_ByteString bsAttrDict;
static UA_ByteString bsAttrDesc;


static UA_NodeId myStructVariableTypeId;

typedef struct
{
   UA_Int32 attributeA;
   UA_Boolean attributeB;
}
MyStruct;

static MyStruct MyStructVariable = { 1234, TRUE };

#define padding_attributeB offsetof(MyStruct,attributeB) - offsetof(MyStruct,attributeA) - sizeof(UA_Int32)


static UA_DataTypeMember myStruct_members[2] = 
{
   /* attributeA */
   {
#ifdef UA_ENABLE_TYPENAMES
   .memberName = "attributeA",
#endif
   .memberTypeIndex = UA_TYPES_INT32,
   .padding = 0,
   .namespaceZero = true,
   .isArray = false
   },
   /* attributeB */
   {
#ifdef UA_ENABLE_TYPENAMES
   .memberName = "attributeB",
#endif
   .memberTypeIndex = UA_TYPES_BOOLEAN,
   .padding = padding_attributeB,
   .namespaceZero = true,
   .isArray = false
   }
};

static const UA_NodeId myStructDataTypeNodeId = { 1,UA_NODEIDTYPE_NUMERIC,{ 1 } };

static const UA_DataType MyStructType =
{
#ifdef UA_ENABLE_TYPENAMES
   "MyStruct",                      /* .typeName */
#endif
   {1,UA_NODEIDTYPE_NUMERIC,{1}},   /* .typeId */
   sizeof(MyStruct),                /* .memSize */
   0,                               /* .typeIndex, in the array of custom types */
   2,                               /* .membersSize */
   false,                           /* .builtin */
   true,                            /* .pointerFree */
   false,                           /* .overlayable (depends on endianness and
                                    the absence of padding) */
   0,                               /* .binaryEncodingId, the numeric
                                    identifier used on the wire (the
                                    namespaceindex is from .typeId) */
   myStruct_members
};


UA_Boolean running = true;
static void stopHandler(int sig) {
   UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "received ctrl-c");
   running = false;
}

static UA_StatusCode
readIntVariable(void *handle, const UA_NodeId nodeid, UA_Boolean sourceTimeStamp,
   const UA_NumericRange *range, UA_DataValue *dataValue) 
{
   myInteger += 1;
   UA_Variant_setScalarCopy(&dataValue->value, &myInteger,
      &UA_TYPES[UA_TYPES_INT32]);
   dataValue->hasValue = true;
   return UA_STATUSCODE_GOOD;
}

static UA_StatusCode
writeIntVariable(void *handle, const UA_NodeId nodeid, const UA_Variant *data,
   const UA_NumericRange *range) 
{
   UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
      "Changing the system time is not implemented");
   return UA_STATUSCODE_BADINTERNALERROR;
}

static UA_StatusCode
readMyStructVariable(void *handle, const UA_NodeId nodeid, UA_Boolean sourceTimeStamp,
   const UA_NumericRange *range, UA_DataValue *dataValue)
{
   MyStructVariable.attributeA += 1;
   MyStructVariable.attributeB = !MyStructVariable.attributeB;

   UA_Variant_setScalarCopy(&dataValue->value, &MyStructVariable,
      &MyStructType);
   dataValue->hasValue = true;
   return UA_STATUSCODE_GOOD;
}

static void
addVariable(UA_Server *server) 
{
   /* Define the attribute of the myInteger variable node */
   UA_VariableAttributes attr;
   UA_VariableAttributes_init(&attr);
   attr.description = UA_LOCALIZEDTEXT("en_US", "UINT32 variable");
   attr.displayName = UA_LOCALIZEDTEXT("en_US", "MyUINT32Variable");
   attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
   attr.valueRank = -1;

   /* Add the variable node to the information model */
   UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "the.answer");
   UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, "the answer");
   UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
   UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
   UA_DataSource integerDataSource;
   integerDataSource.handle = NULL;
   integerDataSource.read = readIntVariable;
   integerDataSource.write = writeIntVariable;
   UA_Server_addDataSourceVariableNode(server, myIntegerNodeId, parentNodeId,
      parentReferenceNodeId, myIntegerName,
      UA_NODEID_NULL, attr,
      integerDataSource, NULL);
}

static void addDataTypeEncodingDescriptionDictionarySystemTypes(UA_Server *server)
{
   /* Add DataTypeEncodingType*/
   UA_ObjectTypeAttributes otEncAttr;
   UA_ObjectTypeAttributes_init(&otEncAttr);
   otEncAttr.displayName = UA_LOCALIZEDTEXT("en_US", "DataTypeEncodingType");
   otEncAttr.description = UA_LOCALIZEDTEXT("en_US", "DataTypeEncodingType");
   UA_Server_addObjectTypeNode(server, UA_NODEID_NUMERIC(0, UA_NS0ID_DATATYPEENCODINGTYPE),
      UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
      UA_QUALIFIEDNAME(1, "DataTypeEncodingType"),
      otEncAttr,
      NULL,
      NULL);

   /* Add DataTypeDescriptionType */
   UA_VariableTypeAttributes vtDescAttr;
   UA_VariableTypeAttributes_init(&vtDescAttr);
   vtDescAttr.displayName = UA_LOCALIZEDTEXT("en_US", "DataTypeDescriptionType");
   vtDescAttr.description = UA_LOCALIZEDTEXT("en_US", "DataTypeDescriptionType");
   vtDescAttr.valueRank = -1;
   vtDescAttr.dataType = UA_NODEID_NUMERIC(0, UA_NS0ID_BYTESTRING);
   UA_ByteString vtAttrByteString = UA_BYTESTRING("");
   UA_Variant_setScalar(&vtDescAttr.value, &vtAttrByteString, &UA_TYPES[UA_TYPES_BYTESTRING]);

   UA_Server_addVariableTypeNode(server,
      UA_NODEID_NUMERIC(0, UA_NS0ID_DATATYPEDESCRIPTIONTYPE),
      UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
      UA_QUALIFIEDNAME(0, "DataTypeDescriptionType"),
      UA_NODEID_NUMERIC(0, UA_NS0ID_BYTESTRING),
      vtDescAttr,
      NULL,
      NULL);

   /* Add DataTypeDictionaryType */
   UA_VariableTypeAttributes vtDictAttr;
   UA_VariableTypeAttributes_init(&vtDictAttr);
   vtDictAttr.displayName = UA_LOCALIZEDTEXT("en_US", "DataTypeDictionaryType");
   vtDictAttr.description = UA_LOCALIZEDTEXT("en_US", "DataTypeDictionaryType");
   vtDictAttr.valueRank = -1;
   vtDictAttr.dataType = UA_NODEID_NUMERIC(0, UA_NS0ID_BYTESTRING);
   UA_ByteString vtDictionaryAttrByteString = UA_BYTESTRING("");
   UA_Variant_setScalar(&vtDictAttr.value, &vtDictionaryAttrByteString, &UA_TYPES[UA_TYPES_BYTESTRING]);

   UA_Server_addVariableTypeNode(server,
      UA_NODEID_NUMERIC(0, UA_NS0ID_DATATYPEDICTIONARYTYPE),
      UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
      UA_QUALIFIEDNAME(0, "DataTypeDictionaryType"),
      UA_NODEID_NUMERIC(0, UA_NS0ID_BYTESTRING),
      vtDictAttr,
      NULL,
      NULL);

   /* Add DataTypeSystemType*/
   UA_ObjectTypeAttributes otSystemAttr;
   UA_ObjectTypeAttributes_init(&otSystemAttr);
   otSystemAttr.displayName = UA_LOCALIZEDTEXT("en_US", "DataTypeSystemType");
   otSystemAttr.description = UA_LOCALIZEDTEXT("en_US", "DataTypeSystemType");
   UA_Server_addObjectTypeNode(server, 
      UA_NODEID_NUMERIC(0, UA_NS0ID_DATATYPESYSTEMTYPE),
      UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
      UA_QUALIFIEDNAME(1, "DataTypeSystemType"),
      otSystemAttr,
      NULL,
      NULL);
}

static void addDataTypeSystem(UA_Server *server)
{
   UA_ObjectAttributes oaOpcBinary;
   UA_ObjectAttributes_init(&oaOpcBinary);
   oaOpcBinary.displayName = UA_LOCALIZEDTEXT("en_US", "OPC Binary");
   oaOpcBinary.description = UA_LOCALIZEDTEXT("en_US", "OPC Binary");

   UA_Server_addObjectNode(server,
      UA_NODEID_NUMERIC(0, UA_NS0ID_OPCBINARYSCHEMA_TYPESYSTEM),
      UA_NODEID_NUMERIC(0, UA_NS0ID_DATATYPESFOLDER),
      UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
      UA_QUALIFIEDNAME(0, "OPC Binary"),
      UA_NODEID_NUMERIC(0, UA_NS0ID_DATATYPESYSTEMTYPE),
      oaOpcBinary,
      NULL,
      NULL);

   /* Add DataTypeDictionary variable */
   UA_VariableAttributes vaDictionary;
   UA_VariableAttributes_init(&vaDictionary);
   vaDictionary.displayName = UA_LOCALIZEDTEXT("en_US", "Opc.Ua");
   vaDictionary.valueRank = -1;
   vaDictionary.dataType = UA_TYPES[UA_TYPES_BYTESTRING].typeId;

   bsAttrDict = UA_BYTESTRING("My xml");
   UA_Variant_setScalar(&vaDictionary.value, &bsAttrDict, &UA_TYPES[UA_TYPES_BYTESTRING]);

   UA_NodeId niDictionary;
   UA_Server_addVariableNode(server,
      UA_NODEID_NULL,
      UA_NODEID_NUMERIC(0, UA_NS0ID_OPCBINARYSCHEMA_TYPESYSTEM),
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
      UA_QUALIFIEDNAME(0, "Opc.Ua"),
      UA_NODEID_NUMERIC(0, UA_NS0ID_DATATYPEDICTIONARYTYPE),
      vaDictionary, NULL, &niDictionary);

   /* Add DataTypeDescription variable */
   UA_VariableAttributes vaDescription;
   UA_VariableAttributes_init(&vaDescription);
   vaDescription.displayName = UA_LOCALIZEDTEXT("en_US", "MyStructDataType");
   vaDescription.description = UA_LOCALIZEDTEXT("en_US", "MyStructDataType");
   vaDescription.valueRank = -1;
   vaDescription.dataType = UA_TYPES[UA_TYPES_BYTESTRING].typeId;

   bsAttrDesc = UA_BYTESTRING("MyStructDataType");
   UA_Variant_setScalar(&vaDescription.value, &bsAttrDesc, &UA_TYPES[UA_TYPES_BYTESTRING]);

   UA_Server_addVariableNode(server,
      UA_NODEID_NULL,
      niDictionary,
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
      UA_QUALIFIEDNAME(0, "MyStructDataType"),
      UA_NODEID_NUMERIC(0, UA_NS0ID_DATATYPEDESCRIPTIONTYPE),
      vaDescription, NULL, &niDataTypeDesc);
}

static void addMyStructDataType(UA_Server *server)
{
   UA_DataTypeAttributes dtAttr;
   UA_NodeId DataTypeEncodingNodeId;

   UA_DataTypeAttributes_init(&dtAttr);
   dtAttr.description = UA_LOCALIZEDTEXT("en_US", "myStructDataType description");
   dtAttr.displayName = UA_LOCALIZEDTEXT("en_US", "myStructDataType");

   UA_Server_addDataTypeNode(server, myStructDataTypeNodeId,
      UA_NODEID_NUMERIC(0, UA_NS0ID_STRUCTURE),
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
      UA_QUALIFIEDNAME(1, "MyStructDataType"),
      dtAttr,
      NULL,
      NULL);

   /* Add DataTypeEncoding object Default Binary */
   UA_ObjectAttributes oAttr;
   UA_ObjectAttributes_init(&oAttr);
   oAttr.displayName = UA_LOCALIZEDTEXT("en_US", "Default Binary");
   oAttr.description = UA_LOCALIZEDTEXT("en_US", "Default Binary");

   UA_Server_addObjectNode(server,
      UA_NODEID_NULL,
      UA_NODEID_NULL,
      UA_NODEID_NULL,
      UA_QUALIFIEDNAME(0, "Default Binary"),
      UA_NODEID_NUMERIC(0, UA_NS0ID_DATATYPEENCODINGTYPE),
      oAttr, NULL, &DataTypeEncodingNodeId);

   UA_Server_addReference(server,
      myStructDataTypeNodeId,
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASENCODING),
      UA_EXPANDEDNODEID_NUMERIC(DataTypeEncodingNodeId.namespaceIndex, DataTypeEncodingNodeId.identifier.numeric),
      TRUE);

   UA_Server_addReference(server,
      DataTypeEncodingNodeId,
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASDESCRIPTION),
      UA_EXPANDEDNODEID_NUMERIC(niDataTypeDesc.namespaceIndex, niDataTypeDesc.identifier.numeric),
      TRUE);
}

static void addMyStructVariableType(UA_Server *server)
{
   UA_VariableTypeAttributes vtAttr;
   UA_VariableTypeAttributes_init(&vtAttr);
   vtAttr.dataType = myStructDataTypeNodeId;
   vtAttr.valueRank = -1;
   vtAttr.arrayDimensionsSize = 0;
   vtAttr.displayName = UA_LOCALIZEDTEXT("en_US", "MyStructType");
   vtAttr.description = UA_LOCALIZEDTEXT("en_US", "MyStructType Description");

   MyStruct defaultValue = { 0, 0 };
   UA_Variant_setScalar(&vtAttr.value, &defaultValue, &MyStructType);

   UA_StatusCode statusCode = UA_Server_addVariableTypeNode(server, UA_NODEID_NULL,
                                                            UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                                            UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                                            UA_QUALIFIEDNAME(1, "MyStructType"), 
                                                            MyStructType.typeId,
                                                            vtAttr, NULL, &myStructVariableTypeId);
}

addMyStructVariable(UA_Server *server)
{
   /* Define the attribute of the myInteger variable node */
   UA_VariableAttributes attr;
   UA_VariableAttributes_init(&attr);
   attr.description = UA_LOCALIZEDTEXT("en_US", "MyStructType variable");
   attr.displayName = UA_LOCALIZEDTEXT("en_US", "MyStructTypeVariable");
   attr.dataType = MyStructType.typeId;
   attr.valueRank = -1;

   /* Add the variable node to the information model */
   UA_NodeId myStructNodeId = UA_NODEID_STRING(1, "myStructVariable");
   UA_QualifiedName myStructName = UA_QUALIFIEDNAME(1, "myStructVariableName");
   UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
   UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
   UA_DataSource myStructDataSource;
   myStructDataSource.handle = NULL;
   myStructDataSource.read = readMyStructVariable;
   myStructDataSource.write = NULL;
   UA_Server_addDataSourceVariableNode(server, myStructNodeId, parentNodeId,
      parentReferenceNodeId, myStructName,
      myStructVariableTypeId, attr,
      myStructDataSource, NULL);
}

int main(void) 
{
   signal(SIGINT, stopHandler);
   signal(SIGTERM, stopHandler);

   UA_ServerConfig config = UA_ServerConfig_standard;
   UA_ServerNetworkLayer nl =
      UA_ServerNetworkLayerTCP(UA_ConnectionConfig_standard, 4840);
   
   config.networkLayers = &nl;
   config.networkLayersSize = 1;

   config.customDataTypesSize = 1;
   config.customDataTypes = &MyStructType;

   UA_Server *server = UA_Server_new(config);

   addVariable(server);
   addDataTypeEncodingDescriptionDictionarySystemTypes(server);
   addDataTypeSystem(server);
   addMyStructDataType(server);
   addMyStructVariableType(server);
   addMyStructVariable(server);

   UA_Server_run(server, &running);
   UA_Server_delete(server);
   nl.deleteMembers(&nl);
   return 0;
}