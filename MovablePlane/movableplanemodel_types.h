#ifndef MOVABLEPLANEMODEL_TYPES_H
#define MOVABLEPLANEMODEL_TYPES_H

namespace MovablePlaneModel
{

enum EN_ItemIds
{
    ENID_FIRST = 0,

    ENID_NAME = ENID_FIRST,

    ENID_COORD,
    ENID_OX,
    ENID_OY,
    ENID_OZ,

    ENID_ROTATE,
    ENID_RX,
    ENID_RY,
    ENID_RZ,

    ENID_SIZE,
    ENID_WIDTH,
    ENID_HEIGHT,

    ENID_LAST
};
typedef int TItemId;

}

#endif // MOVABLEPLANEMODEL_TYPES_H
