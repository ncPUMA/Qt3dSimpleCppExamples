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

static const QSizeF DEF_SIZE = QSizeF(1.f, 1.f);
static const float DEF_ALPHA = .5f;
static const QColor DEF_A_CLR = QColor(Qt::green);
static const QColor DEF_B_CLR = QColor(Qt::red);
static const QColor DEF_BORDER_CLR = QColor(Qt::blue);
static const float DEF_BORDER_RADIUS = 0.005f;
static const QColor DEF_SPHERE_CLR = QColor(Qt::gray);

inline static Qt3DCore::QTransform& createBorder(Qt3DCore::QEntity &parent,
                                                 Qt3DRender::QGeometryRenderer &render,
                                                 Qt3DRender::QMaterial &material)
{
    Qt3DCore::QEntity * const border = new Qt3DCore::QEntity(&parent);
    Qt3DCore::QTransform * const transform = new Qt3DCore::QTransform(border);
    border->addComponent(transform);
    border->addComponent(&render);
    border->addComponent(&material);
    return *transform;
}

inline static Qt3DCore::QTransform& createSphere(Qt3DCore::QEntity &parent,
                                                 Qt3DRender::QGeometryRenderer &render,
                                                 Qt3DRender::QMaterial &material)
{
    Qt3DCore::QEntity * const sphere = new Qt3DCore::QEntity(&parent);
    Qt3DCore::QTransform * const transform = new Qt3DCore::QTransform(sphere);
    sphere->addComponent(transform);
    sphere->addComponent(&render);
    sphere->addComponent(&material);
    return *transform;
}

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


class CMovablePlanePrivate
{
    friend class CMovablePlane;

    CMovablePlanePrivate(CMovablePlane * const qptr)
    {
        //planes
        Qt3DCore::QEntity * const sideA = new Qt3DCore::QEntity(qptr);
        Qt3DExtras::QPlaneMesh * const sideAMesh = new Qt3DExtras::QPlaneMesh(sideA);
        sideAMesh->setWidth(DEF_SIZE.width());
        sideAMesh->setHeight(DEF_SIZE.height());
        Qt3DExtras::QPhongAlphaMaterial * const sideAMaterial = new Qt3DExtras::QPhongAlphaMaterial(sideA);
        sideAMaterial->setObjectName("sideAMaterial");
        sideAMaterial->setAmbient(DEF_A_CLR);
        sideAMaterial->setAlpha(DEF_ALPHA);
        sideA->addComponent(sideAMesh);
        sideA->addComponent(sideAMaterial);

        Qt3DCore::QEntity * const sideB = new Qt3DCore::QEntity(qptr);
        Qt3DCore::QTransform * const sideBTr = new Qt3DCore::QTransform(sideB);
        sideBTr->setRotationX(180.f);
        Qt3DExtras::QPlaneMesh * const sideBMesh = new Qt3DExtras::QPlaneMesh(sideB);
        sideBMesh->setWidth(DEF_SIZE.width());
        sideBMesh->setHeight(DEF_SIZE.height());
        Qt3DExtras::QPhongAlphaMaterial * const sideBMaterial = new Qt3DExtras::QPhongAlphaMaterial(sideB);
        sideAMaterial->setObjectName("sideBMaterial");
        sideBMaterial->setAmbient(DEF_B_CLR);
        sideBMaterial->setAlpha(DEF_ALPHA);
        sideB->addComponent(sideBTr);
        sideB->addComponent(sideBMesh);
        sideB->addComponent(sideBMaterial);

        createBorders(*sideA);
        createSpheres(*sideA);
    }

    void createBorders(Qt3DCore::QEntity &sideA)
    {
        Qt3DExtras::QCylinderMesh * const mesh = new Qt3DExtras::QCylinderMesh(&sideA);
        mesh->setShareable(true);
        mesh->setRadius(DEF_BORDER_RADIUS);
        mesh->setLength(DEF_SIZE.height());
        mesh->setSlices(10);
        mesh->setRings(2);
        Qt3DExtras::QPhongAlphaMaterial * const material = new Qt3DExtras::QPhongAlphaMaterial(mesh);
        material->setObjectName("borderMaterial");
        material->setShareable(true);
        material->setAmbient(DEF_BORDER_CLR);
        material->setAlpha(DEF_ALPHA);
        removeNoDepthTest(*material);

        const float antiRotationAngle = 90.f;
        //left
        Qt3DCore::QTransform &lftTr = createBorder(sideA, *mesh, *material);
        lftTr.setRotationX(antiRotationAngle);
        lftTr.setTranslation(QVector3D( - DEF_SIZE.width() / 2, .0f, .0f));
        //right
        Qt3DCore::QTransform &rightTr = createBorder(sideA, *mesh, *material);
        rightTr.setRotationX(antiRotationAngle);
        rightTr.setTranslation(QVector3D(DEF_SIZE.width() / 2, .0f, .0f));
        //top
        Qt3DCore::QTransform &topTr = createBorder(sideA, *mesh, *material);
        topTr.setRotationZ(antiRotationAngle);
        topTr.setTranslation(QVector3D(0.f, 0.f, - DEF_SIZE.height() / 2));
        //bottom
        Qt3DCore::QTransform &bottomTr = createBorder(sideA, *mesh, *material);
        bottomTr.setRotationZ(antiRotationAngle);
        bottomTr.setTranslation(QVector3D(0.f, 0.f, DEF_SIZE.height() / 2));
    }

    void createSpheres(Qt3DCore::QEntity &sideA)
    {
        Qt3DExtras::QCuboidMesh * const mesh = new Qt3DExtras::QCuboidMesh(&sideA);
        mesh->setShareable(true);
        mesh->setXExtent(DEF_BORDER_RADIUS * 4);
        mesh->setYExtent(DEF_BORDER_RADIUS * 4);
        mesh->setZExtent(DEF_BORDER_RADIUS * 4);
        Qt3DExtras::QPhongAlphaMaterial * const material = new Qt3DExtras::QPhongAlphaMaterial(mesh);
        material->setObjectName("sphereMaterial");
        material->setShareable(true);
        material->setAmbient(DEF_SPHERE_CLR);
        material->setAlpha(1.f);
        removeNoDepthTest(*material);

        static const std::vector < std::pair <float, float> > deltas = {
            { - DEF_SIZE.width() / 2, - DEF_SIZE.height() / 2 },
            {                    .0f, - DEF_SIZE.height() / 2 },
            {   DEF_SIZE.width() / 2, - DEF_SIZE.height() / 2 },
            { - DEF_SIZE.width() / 2, .0f                     },
            {   DEF_SIZE.width() / 2, .0f                     },
            { - DEF_SIZE.width() / 2,   DEF_SIZE.height() / 2 },
            {                    0.f,   DEF_SIZE.height() / 2 },
            {   DEF_SIZE.width() / 2,   DEF_SIZE.height() / 2 }
        };
        for(auto &pair : deltas)
        {
            Qt3DCore::QTransform &tr = createSphere(sideA, *mesh, *material);
            tr.setTranslation(QVector3D(pair.first, .0f, pair.second));
        }
    }
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
                              const QColor &border, const QColor &spheres)
{
    Q_ASSERT(setMaterialColor(*this, sideA, "sideAMaterial"));
    Q_ASSERT(setMaterialColor(*this, sideB, "sideBMaterial"));
    Q_ASSERT(setMaterialColor(*this, border, "borderMaterial"));
    Q_ASSERT(setMaterialColor(*this, spheres, "sphereMaterial"));
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
    Q_ASSERT(setMaterialColor(*this, alpha, "sideAMaterial"));
    Q_ASSERT(setMaterialColor(*this, alpha, "sideBMaterial"));
    Q_ASSERT(setMaterialColor(*this, alpha, "borderMaterial"));
}
