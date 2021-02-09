#ifndef CMOVABLEPLANEMODEL_H
#define CMOVABLEPLANEMODEL_H

#include "cabstractentitymodel.h"

class CMovablePlane;
class CMovablePlaneModelPrivate;

class CMovablePlaneModel : public CAbstractEntityModel
{
    Q_OBJECT

    friend class CMovablePlaneDelegate;

public:
    CMovablePlaneModel(CMovablePlane &plane);
    ~CMovablePlaneModel();

protected:
    int rowCountPrivate(const QModelIndex &parent) const override;
    QModelIndex parentPrivate(const QModelIndex &index) const override;
    QModelIndex indexPrivate(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QString displayRoleData(const QModelIndex &index) const override;
    QVariant userRoleData(const QModelIndex &index) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setEditRoleData(const QModelIndex &index, const QVariant &value) override;

private slots:
    void slNameChanged();
    void slTransformChanged();
    void slSizeChanged();

private:
    CMovablePlaneModelPrivate * const d_ptr;
};

#endif // CMOVABLEPLANEMODEL_H
