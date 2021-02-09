#include "cobjinspectorfactorymethod.h"

#include <map>
#include <typeindex>
#include <functional>

#include <QAbstractItemModel>
#include <QAbstractItemDelegate>

#include <Qt3DCore/QEntity>

#include "cmovableplane.h"
#include "cmovableplanemodel.h"
#include "cmovableplanedelegate.h"

static std::map <std::type_index, std::function <QAbstractItemModel *(Qt3DCore::QEntity &entity)> > models;
static std::map <std::type_index, std::function <QAbstractItemDelegate *()> > delegates;

template <typename TEntity, typename TModel, typename TDelegate>
inline static void registerConstructors() {
    models[std::type_index(typeid(TEntity))] =
            [](Qt3DCore::QEntity &entity){ return new TModel(*reinterpret_cast <TEntity *> (&entity)); };
    delegates[std::type_index(typeid(TEntity))] = [](){ return new TDelegate(); };
}

inline static bool registration() {
    registerConstructors <CMovablePlane, CMovablePlaneModel, CMovablePlaneDelegate> ();
//add new models here
    return true;
};

QAbstractItemModel *CObjInspectorFactoryMethod::createModel(Qt3DCore::QEntity &entity)
{
    static const bool reg = registration();
    (void)reg;

    QAbstractItemModel *result = nullptr;
    const std::type_index index = std::type_index(typeid(entity));
    auto indexIt = models.find(index);
    if(indexIt != models.cend())
        result = indexIt->second(entity);
    return result;
}

QAbstractItemDelegate *CObjInspectorFactoryMethod::createDelegate(Qt3DCore::QEntity &entity)
{
    QAbstractItemDelegate *result = nullptr;
    const std::type_index index = std::type_index(typeid(entity));
    auto indexIt = delegates.find(index);
    if(indexIt != delegates.cend())
        result = indexIt->second();
    return result;
}
