#include "cmovableplanemodel.h"

#include <set>

#include <Qt3DCore/QTransform>

#include "cmovableplane.h"
#include "movableplanemodel_types.h"

using namespace MovablePlaneModel;

struct SItem
{
    SItem(const TItemId itemId, const QString &txt) :
        id(itemId),
        text(txt),
        parent(nullptr)
    { }

    ~SItem() { }

    SItem(const SItem &&other) :
        id(other.id),
        text(other.text),
        parent(other.parent)
    { }

    SItem(const SItem &) = delete;
    SItem& operator =(const SItem &) = delete;
    SItem& operator =(const SItem &&) = delete;

    const TItemId id;
    const QString text;
    SItem *parent;
} treeModel[] = {
    SItem(ENID_NAME  , CMovablePlaneModel::tr("Наименование")),
    SItem(ENID_COORD , CMovablePlaneModel::tr("Координаты"  )),
    SItem(ENID_OX    , CMovablePlaneModel::tr("Ось X"       )),
    SItem(ENID_OY    , CMovablePlaneModel::tr("Ось Y"       )),
    SItem(ENID_OZ    , CMovablePlaneModel::tr("Ось Z"       )),
    SItem(ENID_ROTATE, CMovablePlaneModel::tr("Вращение"    )),
    SItem(ENID_RX    , CMovablePlaneModel::tr("Ось X"       )),
    SItem(ENID_RY    , CMovablePlaneModel::tr("Ось Y"       )),
    SItem(ENID_RZ    , CMovablePlaneModel::tr("Ось Z"       )),
    SItem(ENID_SIZE  , CMovablePlaneModel::tr("Размер"      )),
    SItem(ENID_WIDTH , CMovablePlaneModel::tr("Ширина"      )),
    SItem(ENID_HEIGHT, CMovablePlaneModel::tr("Глубина"     ))
};

static const bool initTree = [](){
    Q_ASSERT(sizeof(treeModel) == sizeof(SItem) * ENID_LAST);
    treeModel[ENID_OX].parent     = &treeModel[ENID_COORD];
    treeModel[ENID_OY].parent     = &treeModel[ENID_COORD];
    treeModel[ENID_OZ].parent     = &treeModel[ENID_COORD];
    treeModel[ENID_RX].parent     = &treeModel[ENID_ROTATE];
    treeModel[ENID_RY].parent     = &treeModel[ENID_ROTATE];
    treeModel[ENID_RZ].parent     = &treeModel[ENID_ROTATE];
    treeModel[ENID_WIDTH].parent  = &treeModel[ENID_SIZE];
    treeModel[ENID_HEIGHT].parent = &treeModel[ENID_SIZE];
    return true;
}();

class CMovablePlaneModelPrivate
{
    friend class CMovablePlaneModel;

    CMovablePlaneModelPrivate(CMovablePlane &planeEntity) :
        plane(&planeEntity) { }

    const Qt3DCore::QTransform& transform() const {
        const QVector <Qt3DCore::QTransform *> trVec =
                plane->componentsOfType <Qt3DCore::QTransform> ();
        if (trVec.empty() == false)
            return *trVec.front();
        return defTr;
    }

    Qt3DCore::QTransform& transform() {
        const QVector <Qt3DCore::QTransform *> trVec =
                plane->componentsOfType <Qt3DCore::QTransform> ();
        if (trVec.empty() == false)
            return *trVec.front();
        return defTr;
    }

private:
    CMovablePlane * const plane;
    Qt3DCore::QTransform defTr;
};



CMovablePlaneModel::CMovablePlaneModel(CMovablePlane &plane) :
    CAbstractEntityModel(plane),
    d_ptr(new CMovablePlaneModelPrivate(plane))
{
    connect(d_ptr->plane, SIGNAL(sigNameChanged(QString)), SLOT(slNameChanged()));
    connect(&d_ptr->transform(), SIGNAL(translationChanged(QVector3D)), SLOT(slTransformChanged()));
    connect(&d_ptr->transform(), SIGNAL(rotationChanged(QQuaternion)), SLOT(slTransformChanged()));
    connect(d_ptr->plane, SIGNAL(sigSizeChanged()), SLOT(slSizeChanged()));
}

CMovablePlaneModel::~CMovablePlaneModel()
{
    delete d_ptr;
}

int CMovablePlaneModel::rowCountPrivate(const QModelIndex &parent) const
{
    int res = 0;
    if (parent.isValid() == false)
        res = ENID_LAST;
    else
    {
        SItem *itm = reinterpret_cast <SItem *> (parent.internalPointer());
        for(int i = ENID_FIRST; i < ENID_LAST; ++i)
            if(treeModel[i].parent == itm)
                ++res;
    }
    return res;
}

QModelIndex CMovablePlaneModel::parentPrivate(const QModelIndex &index) const
{
    QModelIndex result;
    if (index.isValid())
    {
        SItem *itm = reinterpret_cast <SItem *> (index.internalPointer());
        int row = 0;
        for(int i = ENID_FIRST; i < ENID_LAST; ++i)
        {
            if(&treeModel[i] == itm->parent)
            {
                result = createIndex(row, ENC_FIRST, &treeModel[i]);
                break;
            }
            else
                ++row;
        }
    }
    return result;
}

QModelIndex CMovablePlaneModel::indexPrivate(int row, int column, const QModelIndex &parent) const
{
    QModelIndex result;
    const SItem *itm = nullptr;
    if (parent.isValid())
        itm = reinterpret_cast <SItem *> (parent.internalPointer());
    int curRow = 0;
    for(int i = ENID_FIRST; i < ENID_LAST; ++i)
    {
        if (treeModel[i].parent == itm)
        {
            if (curRow == row)
            {
                result = createIndex(row, column, &treeModel[i]);
                break;
            }
            else
                ++curRow;
        }
    }
    return result;
}

QString CMovablePlaneModel::displayRoleData(const QModelIndex &index) const
{
    QString res;
    if (index.isValid())
    {
        SItem *itm = reinterpret_cast <SItem *> (index.internalPointer());
        switch(index.column())
        {
            case ENC_NAME:
                res = itm->text;
                break;
            case ENC_VALUE:
                switch(itm->id)
                {
                    case ENID_NAME:
                        res = d_ptr->plane->getName();
                        break;
                    case ENID_OX:
                        res = QString("%1")
                                .arg(d_ptr->transform().translation().x(), 0, 'f', 6);
                        break;
                    case ENID_OY:
                        res = QString("%1")
                                .arg(d_ptr->transform().translation().y(), 0, 'f', 6);
                        break;
                    case ENID_OZ:
                        res = QString("%1")
                                .arg(d_ptr->transform().translation().z(), 0, 'f', 6);
                        break;
                    case ENID_RX:
                        res = QString("%1")
                                .arg(d_ptr->transform().rotationX(), 0, 'f', 3);
                        break;
                    case ENID_RY:
                        res = QString("%1")
                                .arg(d_ptr->transform().rotationY(), 0, 'f', 3);
                        break;
                    case ENID_RZ:
                        res = QString("%1")
                                .arg(d_ptr->transform().rotationZ(), 0, 'f', 3);
                        break;
                    case ENID_WIDTH:
                        res = QString("%1")
                                .arg(d_ptr->plane->getSize().width(), 0, 'f', 3);
                        break;
                    case ENID_HEIGHT:
                        res = QString("%1")
                                .arg(d_ptr->plane->getSize().height(), 0, 'f', 3);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    return res;
}

QVariant CMovablePlaneModel::userRoleData(const QModelIndex &index) const
{
    QVariant res;
    if (index.isValid())
    {
        SItem *itm = reinterpret_cast <SItem *> (index.internalPointer());
        res = itm->id;
    }
    return res;
}

Qt::ItemFlags CMovablePlaneModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags result = Qt::ItemIsEnabled;
    if (index.isValid() && index.column() == ENC_VALUE)
    {
        SItem *itm = reinterpret_cast <SItem *> (index.internalPointer());
        switch(itm->id)
        {
            case ENID_NAME:
            case ENID_OX:
            case ENID_OY:
            case ENID_OZ:
            case ENID_RX:
            case ENID_RY:
            case ENID_RZ:
            case ENID_WIDTH:
            case ENID_HEIGHT:
                result |= Qt::ItemIsEditable;
                break;
            default:
                break;
        }
    }
    return result;
}

bool CMovablePlaneModel::setEditRoleData(const QModelIndex &index, const QVariant &value)
{
    bool result = true;
    if (index.isValid() && index.column() == ENC_VALUE)
    {
        SItem *itm = reinterpret_cast <SItem *> (index.internalPointer());
        switch(itm->id)
        {
            case ENID_NAME:
                d_ptr->plane->setName(value.toString());
                break;
            case ENID_OX:
            {
                QVector3D pos = d_ptr->transform().translation();
                pos.setX(static_cast <float> (value.toDouble()));
                d_ptr->transform().setTranslation(pos);
                break;
            }
            case ENID_OY:
            {
                QVector3D pos = d_ptr->transform().translation();
                pos.setY(static_cast <float> (value.toDouble()));
                d_ptr->transform().setTranslation(pos);
                break;
            }
            case ENID_OZ:
            {
                QVector3D pos = d_ptr->transform().translation();
                pos.setZ(static_cast <float> (value.toDouble()));
                d_ptr->transform().setTranslation(pos);
                break;
            }
            case ENID_RX:
                d_ptr->transform().setRotationX(static_cast <float> (value.toDouble()));
                break;
            case ENID_RY:
                d_ptr->transform().setRotationY(static_cast <float> (value.toDouble()));
                break;
            case ENID_RZ:
                d_ptr->transform().setRotationZ(static_cast <float> (value.toDouble()));
                break;
            case ENID_WIDTH:
            {
                QSizeF size = d_ptr->plane->getSize();
                size.setWidth(static_cast <float> (value.toDouble()));
                d_ptr->plane->setSize(size);
                break;
            }
            case ENID_HEIGHT:
            {
                QSizeF size = d_ptr->plane->getSize();
                size.setHeight(static_cast <float> (value.toDouble()));
                d_ptr->plane->setSize(size);
                break;
            }
            default:
                result = false;
                break;
        }
    }
    return result;
}

void CMovablePlaneModel::slNameChanged()
{
    const QVector <int> roles = { Qt::DisplayRole };
    const QModelIndex index = createIndex(0, ENC_VALUE, &treeModel[ENID_NAME]);
    emit dataChanged(index, index, roles);
}

void CMovablePlaneModel::slTransformChanged()
{
    const QVector <int> roles = { Qt::DisplayRole };
    for(int i = 0; i < 3/*magic num*/; ++i)
    {
        const QModelIndex posIndex = createIndex(i, ENC_VALUE, &treeModel[ENID_OX + i]);
        emit dataChanged(posIndex, posIndex, roles);
        const QModelIndex rotateIndex = createIndex(i, ENC_VALUE, &treeModel[ENID_RX + i]);
        emit dataChanged(rotateIndex, rotateIndex, roles);
    }
}

void CMovablePlaneModel::slSizeChanged()
{
    const QVector <int> roles = { Qt::DisplayRole };
    const QModelIndex wIndex = createIndex(0, ENC_VALUE, &treeModel[ENID_WIDTH]);
    emit dataChanged(wIndex, wIndex, roles);
    const QModelIndex hIndex = createIndex(1, ENC_VALUE, &treeModel[ENID_HEIGHT]);
    emit dataChanged(hIndex, hIndex, roles);
}
