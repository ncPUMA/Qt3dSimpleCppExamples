#include "cabstractentitymodel.h"

class CAbstractEntityModelPrivate
{
    friend class CAbstractEntityModel;

    CAbstractEntityModelPrivate(Qt3DCore::QEntity &ent) :
        entity(&ent)
    { }

    Qt3DCore::QEntity * const entity;
};

CAbstractEntityModel::CAbstractEntityModel(Qt3DCore::QEntity &entity) :
    QAbstractItemModel(nullptr),
    d_ptr(new CAbstractEntityModelPrivate(entity))
{

}

CAbstractEntityModel::~CAbstractEntityModel()
{
    delete d_ptr;
}

int CAbstractEntityModel::columnCount(const QModelIndex &parent) const
{
    (void)parent;

    return ENC_LAST;
}

int CAbstractEntityModel::rowCount(const QModelIndex &parent) const
{
    return rowCountPrivate(parent);
}

QModelIndex CAbstractEntityModel::parent(const QModelIndex &index) const
{
    return parentPrivate(index);
}

QModelIndex CAbstractEntityModel::index(int row, int column, const QModelIndex &parent) const
{
    return indexPrivate(row, column, parent);
}

QVariant CAbstractEntityModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result = QAbstractItemModel::headerData(section, orientation, role);
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
            case ENC_NAME:
                result = tr("Параметр");
                break;
            case ENC_VALUE:
                result = tr("Значение");
                break;
            default:
                result = QString("%1").arg(section + 1);
                break;
        }
    }
    return result;
}

QVariant CAbstractEntityModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    switch(role)
    {
        case Qt::DisplayRole:
            result = displayRoleData(index);
            break;
        case Qt::UserRole:
            result = userRoleData(index);
            break;
        default:
            break;
    }
    return result;
}

bool CAbstractEntityModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result = false;
    switch(role)
    {
        case Qt::EditRole:
            result = setEditRoleData(index, value);
            break;
        default:
            break;
    }
    return result;
}

QVariant CAbstractEntityModel::userRoleData(const QModelIndex &index) const
{
    (void)index;

    return QVariant();
}
