#ifndef CABSTRACTENTITYMODEL_H
#define CABSTRACTENTITYMODEL_H

#include <QAbstractItemModel>

namespace Qt3DCore { class QEntity; }

class CAbstractEntityModelPrivate;

class CAbstractEntityModel : public QAbstractItemModel
{
public:
    enum EN_Columns
    {
        ENC_FIRST = 0,

        ENC_NAME = ENC_FIRST,
        ENC_VALUE,

        ENC_LAST
    };

public:
    CAbstractEntityModel(Qt3DCore::QEntity &entity);
    ~CAbstractEntityModel();

    int columnCount(const QModelIndex &parent) const final;
    int rowCount(const QModelIndex &parent) const final;

    QModelIndex parent(const QModelIndex &index) const final;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const final;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const final;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) final;

protected:
    virtual int rowCountPrivate(const QModelIndex &parent) const = 0;
    virtual QModelIndex parentPrivate(const QModelIndex &index) const = 0;
    virtual QModelIndex indexPrivate(int row, int column, const QModelIndex &parent = QModelIndex()) const = 0;

    virtual QString displayRoleData(const QModelIndex &index) const = 0;
    virtual QVariant userRoleData(const QModelIndex &index) const;

    virtual bool setEditRoleData(const QModelIndex &index, const QVariant &value) = 0;

private:
    CAbstractEntityModelPrivate * const d_ptr;
};

#endif // CABSTRACTENTITYMODEL_H
