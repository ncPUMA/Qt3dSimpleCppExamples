#ifndef COBJINSPECTORFACTORYMETHOD_H
#define COBJINSPECTORFACTORYMETHOD_H

namespace Qt3DCore { class QEntity; }

class QAbstractItemModel;
class QAbstractItemDelegate;

class CObjInspectorFactoryMethod
{
public:
    static QAbstractItemModel* createModel(Qt3DCore::QEntity &entity);
    static QAbstractItemDelegate* createDelegate(Qt3DCore::QEntity &entity);

private:
    CObjInspectorFactoryMethod() = delete;
};

#endif // COBJINSPECTORFACTORYMETHOD_H
