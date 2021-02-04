#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QVBoxLayout>
#include <QToolBar>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QMesh>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>

#include <Qt3DExtras/QMetalRoughMaterial>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QPointLight>

#include "cmovableplane.h"

using namespace Qt3DRender;

class MainWindowPrivate
{
    friend class MainWindow;

    MainWindowPrivate() :
        scene(nullptr),
        model(nullptr),
        view(nullptr)
    { }

    Qt3DCore::QEntity *scene;
    Qt3DCore::QEntity *model;
    Qt3DExtras::Qt3DWindow *view;
};


//camera
static const QVector3D DEF_CAM_POS = QVector3D(0.f, 0.f, 1.5f);
static const QVector3D DEF_CAM_VCENTR = QVector3D(0.f, 0.f, 0.f);

//perspective projection
static const float FIELD_VIEW = 60.f;
static const float ASPECT_RATIO = 16.f / 9.f;
static const float NEAR_PLAN = .1f;
static const float FAR_PLAN = 1000.f;

//model
static const char* MDL_OBJ_PATH = "qrc:/v8_engine.obj";
static const float DEF_Y_ROTATION = - 90.f;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , d_ptr(new MainWindowPrivate())
{
    ui->setupUi(this);

    connect(ui->pbAddPlane, SIGNAL(clicked(bool)), SLOT(slAddPlane()));

    //Build main scene
    d_ptr->scene = new Qt3DCore::QEntity();

    d_ptr->model = new Qt3DCore::QEntity(d_ptr->scene);
    Qt3DCore::QTransform * const rotateY = new Qt3DCore::QTransform(d_ptr->model);
    rotateY->setRotationY(DEF_Y_ROTATION);
    d_ptr->model->addComponent(rotateY);
    QMesh * const loader = new QMesh(d_ptr->model);
    loader->setSource(QUrl(MDL_OBJ_PATH));
    Qt3DExtras::QMetalRoughMaterial * const mdlMaterial =
            new Qt3DExtras::QMetalRoughMaterial(d_ptr->model);
    mdlMaterial->setBaseColor(QColor(Qt::gray));
    d_ptr->model->addComponent(loader);
    d_ptr->model->addComponent(mdlMaterial);

    //Install viewport camera, and light
    d_ptr->view = new Qt3DExtras::Qt3DWindow();
    QVBoxLayout * const mainLay = new QVBoxLayout(ui->centralwidget);
    mainLay->addWidget(createWindowContainer(d_ptr->view, ui->centralwidget));

    d_ptr->view->camera()->lens()->setPerspectiveProjection(FIELD_VIEW, ASPECT_RATIO,
                                                            NEAR_PLAN, FAR_PLAN);

    d_ptr->view->camera()->setPosition(DEF_CAM_POS);
    d_ptr->view->camera()->setViewCenter(DEF_CAM_VCENTR);

    d_ptr->view->setRootEntity(d_ptr->scene);

    Qt3DExtras::QOrbitCameraController * const camerController =
            new Qt3DExtras::QOrbitCameraController(d_ptr->view->camera());
    camerController->setCamera(d_ptr->view->camera());
//    camerController->setEnabled(false);

    Qt3DRender::QPointLight * const lightSrc =
            new Qt3DRender::QPointLight(d_ptr->view->camera());
    lightSrc->setConstantAttenuation(.7f);
    d_ptr->view->camera()->addComponent(lightSrc);
}

MainWindow::~MainWindow()
{
    delete d_ptr->scene;
    delete d_ptr;
    delete ui;
}

void MainWindow::slAddPlane()
{
    CMovablePlane * const plane = new CMovablePlane(d_ptr->scene);
    Qt3DCore::QTransform * const transform = new Qt3DCore::QTransform(plane);
    transform->setTranslation(QVector3D( - .5f, .5f, 0.f));
    transform->setRotationX(90.f);
//    transform->setRotationZ(55.f);
    plane->addComponent(transform);
    connect(plane, SIGNAL(sigMouseHovered(bool)), SLOT(slMouseUnderPlane(bool)));
}

void MainWindow::slMouseUnderPlane(bool contains)
{
    const Qt::CursorShape shape = contains ? Qt::OpenHandCursor : Qt::ArrowCursor;
    QCursor cur = cursor();
    cur.setShape(shape);
    setCursor(cur);
}

