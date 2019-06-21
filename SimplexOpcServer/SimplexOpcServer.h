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

    typedef struct {
        UA_Float x;
        UA_Float y;
        UA_Float z;
    } Point;

    /* The datatype description for the Point datatype */
    #define Point_padding_y offsetof(Point,y) - offsetof(Point,x) - sizeof(UA_Float)
    #define Point_padding_z offsetof(Point,z) - offsetof(Point,y) - sizeof(UA_Float)

    UA_DataTypeMember Point_members[3] = {
            /* x */
            {
                    UA_TYPENAME("x") /* .memberName */
                    UA_TYPES_FLOAT,  /* .memberTypeIndex, points into UA_TYPES since namespaceZero is true */
                    0,               /* .padding */
                    true,            /* .namespaceZero, see .memberTypeIndex */
                    false            /* .isArray */
            },

            /* y */
            {
                    UA_TYPENAME("y")
                    UA_TYPES_FLOAT, Point_padding_y, true, false
            },

            /* z */
            {
                    UA_TYPENAME("z")
                    UA_TYPES_FLOAT, Point_padding_z, true, false
            }
    };

    UA_DataType PointType = {
            UA_TYPENAME("Point")             /* .typeName */
            {1, UA_NODEIDTYPE_NUMERIC, {4242}}, /* .typeId */
            sizeof(Point),                   /* .memSize */
            0,                               /* .typeIndex, in the array of custom types */
            UA_DATATYPEKIND_STRUCTURE,       /* .typeKind */
            true,                            /* .pointerFree */
            false,                           /* .overlayable (depends on endianness and
                                             the absence of padding) */
            3,                               /* .membersSize */
            4244,                            /* .binaryEncodingId, the numeric
                                             identifier used on the wire (the
                                             namespaceindex is from .typeId) */
            Point_members
    };

};

class MyDataType : public QUaBaseDataVariable
{
    Q_OBJECT

};

