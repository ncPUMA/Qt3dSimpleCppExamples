#include "cmovableplane.h"

#include <vector>
#include <utility>

#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>

#include <Qt3DCore/QTransform>

#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>

static const QSizeF DEF_SIZE = QSizeF(1.f, 1.f);
static const float DEF_ALPHA = .5f;
static const QColor DEF_A_CLR = QColor(Qt::green);
static const QColor DEF_B_CLR = QColor(Qt::red);
static const QColor DEF_BORDER_CLR = QColor(Qt::blue);
static const float DEF_BORDER_RADIUS = 0.005f;
static const QColor DEF_SPHERE_CLR = QColor(Qt::gray);

enum EN_Points
{
    ENP_FIRST = 0,

    ENP_TOP_LEFT = ENP_FIRST,
    ENP_TOP,
    ENP_TOP_RIGHT,
    ENP_LEFT,
    ENP_BOTTOM_LEFT,
    ENP_BOTTOM,
    ENP_BOTTOM_RIGHT,
    ENP_RIGHT,

    ENP_LAST
};
typedef int TPoint;

inline static void removeNoDepthTest(Qt3DRender::QMaterial &material)
{
    foreach(Qt3DRender::QTechnique * const tech, material.effect()->techniques())
        foreach(Qt3DRender::QRenderPass * const pass, tech->renderPasses())
            foreach(Qt3DRender::QRenderState * const st, pass->renderStates())
            {
                Qt3DRender::QNoDepthMask * const dMask =
                        qobject_cast <Qt3DRender::QNoDepthMask *> (st);
                if (dMask)
                {
                    pass->removeRenderState(dMask);
                    dMask->deleteLater();
                    continue;
                }
            }
}

inline static void removeTransform(Qt3DCore::QEntity &parent)
{
    foreach(Qt3DCore::QTransform * const tr, parent.componentsOfType <Qt3DCore::QTransform> ())
    {
        parent.removeComponent(tr);
        tr->deleteLater();
    }
}


class CMovablePlanePrivate
{
    friend class CMovablePlane;

    CMovablePlanePrivate(CMovablePlane * const qptr) :
        size(DEF_SIZE),
        sideA(new Qt3DCore::QEntity(qptr)),
        sideAMesh(new Qt3DExtras::QPlaneMesh(sideA)),
        sideAMaterial(new Qt3DExtras::QPhongAlphaMaterial(sideA)),
        sideB(new Qt3DCore::QEntity(qptr)),
        sideBMesh(new Qt3DExtras::QPlaneMesh(sideB)),
        sideBMaterial(new Qt3DExtras::QPhongAlphaMaterial(sideB)),
        borderMaterial(new Qt3DExtras::QPhongAlphaMaterial(sideA)),
        cubeMesh(new Qt3DExtras::QCuboidMesh(sideA)),
        cubeMaterial(new Qt3DExtras::QPhongAlphaMaterial(sideA))
    {
        //planes
        sideAMesh->setWidth(size.width());
        sideAMesh->setHeight(size.height());
        sideAMaterial->setAmbient(DEF_A_CLR);
        sideAMaterial->setAlpha(DEF_ALPHA);
        sideA->addComponent(sideAMesh);
        sideA->addComponent(sideAMaterial);
        sideBMesh->setWidth(size.width());
        sideBMesh->setHeight(size.height());
        sideBMaterial->setAmbient(DEF_B_CLR);
        sideBMaterial->setAlpha(DEF_ALPHA);
        sideB->addComponent(sideBMesh);
        sideB->addComponent(sideBMaterial);
        updatePlaneTransform();

        //borders
        borderMaterial->setShareable(true);
        borderMaterial->setAmbient(DEF_BORDER_CLR);
        borderMaterial->setAlpha(DEF_ALPHA);
        removeNoDepthTest(*borderMaterial);
        const std::vector <TPoint> borderPoints = {
            ENP_LEFT,
            ENP_RIGHT,
            ENP_TOP,
            ENP_BOTTOM
        };
        for(auto point : borderPoints) {
            Qt3DCore::QEntity * const border = new Qt3DCore::QEntity(sideA);
            Qt3DExtras::QCylinderMesh * const borderMesh = new Qt3DExtras::QCylinderMesh(border);
            borderMesh->setShareable(true);
            borderMesh->setRadius(DEF_BORDER_RADIUS);
            borderMesh->setSlices(10);
            borderMesh->setRings(2);
            border->addComponent(borderMesh);
            border->addComponent(borderMaterial);
            updateBorderTransform(point, *border, *borderMesh);
            borderMap[point] = std::make_pair(border, borderMesh);
        }

        //cubes
        cubeMesh->setShareable(true);
        cubeMesh->setXExtent(DEF_BORDER_RADIUS * 4);
        cubeMesh->setYExtent(DEF_BORDER_RADIUS * 4);
        cubeMesh->setZExtent(DEF_BORDER_RADIUS * 4);
        cubeMaterial->setShareable(true);
        cubeMaterial->setAmbient(DEF_SPHERE_CLR);
        cubeMaterial->setAlpha(1.f);
        removeNoDepthTest(*cubeMaterial);
        for(TPoint i = ENP_FIRST; i < ENP_LAST; ++i) {
            Qt3DCore::QEntity * const cube = new Qt3DCore::QEntity(sideA);
            cube->addComponent(cubeMesh);
            cube->addComponent(cubeMaterial);
            updateCubeTransform(*cube, i);
            cubeMap[i] = cube;
        }

        //input
        Qt3DRender::QObjectPicker * const picker = new Qt3DRender::QObjectPicker(sideA);
        picker->setObjectName("mover");
        picker->setDragEnabled(true);
        picker->setHoverEnabled(true);
        sideA->addComponent(picker);
        QObject::connect(picker, SIGNAL(containsMouseChanged(bool)),
                         qptr, SLOT(slContainsMouseChanged(bool)));
        QObject::connect(picker, SIGNAL(exited()), qptr, SLOT(slHoverExited()));
    }

    void updatePlaneTransform()
    {
        sideAMesh->setWidth(size.width());
        sideAMesh->setHeight(size.height());
        removeTransform(*sideA);
        Qt3DCore::QTransform * const sideATr = new Qt3DCore::QTransform(sideA);
        sideATr->setTranslation(QVector3D(size.width() / 2, 0.f, size.height() / 2));
        sideA->addComponent(sideATr);
        sideBMesh->setWidth(size.width());
        sideBMesh->setHeight(size.height());
        removeTransform(*sideB);
        Qt3DCore::QTransform * const sideBTr = new Qt3DCore::QTransform(sideB);
        sideBTr->setTranslation(sideATr->translation());
        sideBTr->setRotationX( - 180.f);
        sideB->addComponent(sideBTr);
    }

    void updateBorderTransform(const TPoint point,
                               Qt3DCore::QEntity &border,
                               Qt3DExtras::QCylinderMesh &borderMesh)
    {
        removeTransform(border);
        Qt3DCore::QTransform * const transform = new Qt3DCore::QTransform(&border);
        const float antiRotationAngle = 90.f;
        switch(point)
        {
            case ENP_LEFT:
                transform->setRotationX( - antiRotationAngle);
                transform->setTranslation(QVector3D( - size.width() / 2, .0f, .0f));
                borderMesh.setLength(size.height());
                break;
            case ENP_RIGHT:
                transform->setRotationX( - antiRotationAngle);
                transform->setTranslation(QVector3D(size.width() / 2, .0f, .0f));
                borderMesh.setLength(size.height());
                break;
            case ENP_TOP:
                transform->setRotationZ(antiRotationAngle);
                transform->setTranslation(QVector3D(0.f, 0.f, - size.height() / 2));
                borderMesh.setLength(size.width());
                break;
            case ENP_BOTTOM:
                transform->setRotationZ(antiRotationAngle);
                transform->setTranslation(QVector3D(0.f, 0.f, size.height() / 2));
                borderMesh.setLength(size.width());
                break;
            default:
                break;
        }
        border.addComponent(transform);
    }

    void updateCubeTransform(Qt3DCore::QEntity &parent, const TPoint point)
    {
        removeTransform(parent);
        const std::map <TPoint, std::pair <float, float> > deltas = {
            { ENP_TOP_LEFT    , { - size.width() / 2, - size.height() / 2      } },
            { ENP_TOP         , {                     .0f, - size.height() / 2 } },
            { ENP_TOP_RIGHT   , {   size.width() / 2, - size.height() / 2      } },
            { ENP_LEFT        , { - size.width() / 2, .0f                      } },
            { ENP_BOTTOM_LEFT , {   size.width() / 2, .0f                      } },
            { ENP_BOTTOM      , { - size.width() / 2,   size.height() / 2      } },
            { ENP_BOTTOM_RIGHT, {                0.f,   size.height() / 2      } },
            { ENP_RIGHT       , {   size.width() / 2,   size.height() / 2      } }
        };
        auto it = deltas.find(point);
        if (it != deltas.cend()) {
            Qt3DCore::QTransform * const transform = new Qt3DCore::QTransform(&parent);
            transform->setTranslation(QVector3D(it->second.first, .0f, it->second.second));
            parent.addComponent(transform);
        }
    }

    QSizeF size;

    //plane
    Qt3DCore::QEntity * const sideA;
    Qt3DExtras::QPlaneMesh * const sideAMesh;
    Qt3DExtras::QPhongAlphaMaterial * const sideAMaterial;
    Qt3DCore::QEntity * const sideB;
    Qt3DExtras::QPlaneMesh * const sideBMesh;
    Qt3DExtras::QPhongAlphaMaterial * const sideBMaterial;

    //border
    Qt3DExtras::QPhongAlphaMaterial * const borderMaterial;
    std::map <TPoint, std::pair <Qt3DCore::QEntity *, Qt3DExtras::QCylinderMesh *> > borderMap;

    //cubes
    Qt3DExtras::QCuboidMesh * const cubeMesh;
    Qt3DExtras::QPhongAlphaMaterial * const cubeMaterial;
    std::map <TPoint, Qt3DCore::QEntity *> cubeMap;
};



CMovablePlane::CMovablePlane(QNode *parent) :
    Qt3DCore::QEntity(parent),
    d_ptr(new CMovablePlanePrivate(this))
{

}

CMovablePlane::~CMovablePlane()
{
    delete d_ptr;
}

QVector3D CMovablePlane::getPos() const
{
    const QVector <Qt3DCore::QTransform *> trVec = componentsOfType <Qt3DCore::QTransform> ();
    Q_ASSERT(trVec.size() == 1);
    return trVec.front()->translation();
}

inline static bool setMaterialColor(QObject &root, const QColor &clr, const QString &objName)
{
    Qt3DExtras::QPhongAlphaMaterial * const material =
            root.findChild <Qt3DExtras::QPhongAlphaMaterial *> (objName);
    const bool result = (material != nullptr);
    if(result)
        material->setAmbient(clr);
    return result;
}

void CMovablePlane::setColors(const QColor &sideA, const QColor &sideB,
                              const QColor &border, const QColor &cubes)
{
    d_ptr->sideAMaterial->setAmbient(sideA);
    d_ptr->sideBMaterial->setAmbient(sideB);
    d_ptr->borderMaterial->setAmbient(border);
    d_ptr->cubeMaterial->setAmbient(cubes);
}

inline static bool setMaterialAlpha(QObject &root, const float alpha, const QString &objName)
{
    Qt3DExtras::QPhongAlphaMaterial * const material =
            root.findChild <Qt3DExtras::QPhongAlphaMaterial *> (objName);
    const bool result = (material != nullptr);
    if(result)
        material->setAlpha(alpha);
    return result;
}

void CMovablePlane::setAlpha(const float alpha)
{
    d_ptr->sideAMaterial->setAlpha(alpha);
    d_ptr->sideBMaterial->setAlpha(alpha);
    d_ptr->borderMaterial->setAlpha(alpha);
}

void CMovablePlane::slContainsMouseChanged(bool containsMouse)
{
    emit sigMouseHovered(containsMouse);
}

QSizeF CMovablePlane::getSize() const
{
    return d_ptr->size;
}

void CMovablePlane::setPos(const QVector3D &pos)
{
    const QVector <Qt3DCore::QTransform *> trVec = componentsOfType <Qt3DCore::QTransform> ();
    Q_ASSERT(trVec.size() == 1);
    trVec.front()->setTranslation(pos);
}

void CMovablePlane::setSize(const QSizeF &size)
{
    d_ptr->size = size;
    d_ptr->updatePlaneTransform();
    for(auto &pair : d_ptr->borderMap)
        d_ptr->updateBorderTransform(pair.first, *pair.second.first, *pair.second.second);
    for(auto &pair : d_ptr->cubeMap)
        d_ptr->updateCubeTransform(*pair.second, pair.first);
}
