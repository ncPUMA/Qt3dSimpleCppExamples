#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QFile>
#include <QAbstractItemModel>
#include <QAbstractItemDelegate>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QMesh>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QClipPlane>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QPointLight>

#include <Qt3DExtras/QMetalRoughMaterial>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>

#include "cobjinspectorfactorymethod.h"
#include "cmovableplane.h"

using namespace Qt3DRender;

class MainWindowPrivate
{
    friend class MainWindow;

    MainWindowPrivate() :
        scene(nullptr),
        model(nullptr),
        view(nullptr),
        camerController(nullptr),
        plane(nullptr)
    { }

    Qt3DCore::QEntity *scene;
    Qt3DCore::QEntity *model;
    Qt3DExtras::Qt3DWindow *view;
    Qt3DExtras::QOrbitCameraController *camerController;

    CMovablePlane *plane;
    std::vector <Qt3DCore::QEntity *> selectedItems;
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
    std::map <QString, QByteArray> shProgs = {
        { ":/eng.vert", QByteArray() },
        { ":/eng.geom", QByteArray() }
    };
    for(auto &pair : shProgs)
    {
        QFile shaderFile(pair.first);
        if (shaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            shProgs[pair.first] = shaderFile.readAll();;
            shaderFile.close();
        }
        else
            qDebug() << "CClipMaterialPrivate::CClipMaterialPrivate: err load vertex shader"
                     << pair.first << " "
                     << shaderFile.errorString();
    }

    foreach(QTechnique * const technique, mdlMaterial->effect()->techniques())
        foreach(QRenderPass * const pass, technique->renderPasses())
        {
            QShaderProgram * const shProg = pass->shaderProgram();
            shProg->setVertexShaderCode(shProgs[QString(":/eng.vert")]);
            shProg->setGeometryShaderCode(shProgs[QString(":/eng.geom")]);
        }
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

    d_ptr->camerController = new Qt3DExtras::QOrbitCameraController(d_ptr->view->camera());
    d_ptr->camerController->setCamera(d_ptr->view->camera());

    QPointLight * const lightSrc = new QPointLight(d_ptr->view->camera());
    lightSrc->setConstantAttenuation(.7f);
    d_ptr->view->camera()->addComponent(lightSrc);

    //demo
    d_ptr->plane = new CMovablePlane(d_ptr->scene);
    Qt3DCore::QTransform * const transform = new Qt3DCore::QTransform(d_ptr->plane);
    transform->setRotationX(90);
    transform->setTranslation(QVector3D(-0.5f, 0.5f, 0.f));
    d_ptr->plane->addComponent(transform);
    connect(d_ptr->plane, SIGNAL(sigMouseHovered(bool)), SLOT(slMouseUnderPlane(bool)));

    connect(ui->rbCamera, SIGNAL(toggled(bool)), SLOT(slRadioButtonsChanged(bool)));
    connect(ui->rbMove  , SIGNAL(toggled(bool)), SLOT(slRadioButtonsChanged(bool)));
    connect(ui->rbRotate, SIGNAL(toggled(bool)), SLOT(slRadioButtonsChanged(bool)));
    connect(ui->rbResize, SIGNAL(toggled(bool)), SLOT(slRadioButtonsChanged(bool)));

    ui->rbMove->toggle();

    connect(ui->pbClip, SIGNAL(clicked(bool)), SLOT(slPbClip()));

    QParameter * const param = new QParameter(mdlMaterial->effect());
    param->setName("plane");
    param->setValue(QVector4D(0, 0, 0, 0));
    mdlMaterial->effect()->addParameter(param);

    d_ptr->selectedItems.push_back(d_ptr->plane);
    itemsSelectionChanged();
}

MainWindow::~MainWindow()
{
    delete d_ptr->scene;
    delete d_ptr;
    delete ui;
}

void MainWindow::slMouseUnderPlane(bool contains)
{
    const Qt::CursorShape shape = contains ? Qt::OpenHandCursor : Qt::ArrowCursor;
    QCursor cur = cursor();
    cur.setShape(shape);
    setCursor(cur);
}

void MainWindow::slRadioButtonsChanged(bool toggled)
{
    using namespace MovablePlane;

    if (toggled)
    {
        const std::map <QObject *, TTransformationMode> modes = {
            { ui->rbCamera, ENTM_OFF    },
            { ui->rbMove  , ENTM_MOVE   },
            { ui->rbRotate, ENTM_ROTATE },
            { ui->rbResize, ENTM_RESIZE }
        };
        auto it = modes.find(sender());
        if (it != modes.cend())
        {
            d_ptr->plane->setTransformationMode(it->second);
            d_ptr->camerController->setEnabled(it->second == ENTM_OFF);
        }
    }
}

void MainWindow::slPbClip()
{
    auto plTrVec = d_ptr->plane->componentsOfType <Qt3DCore::QTransform> ();
    Q_ASSERT(plTrVec.size() > 0);
    Qt3DCore::QTransform * const transform = plTrVec.front();
    const QVector3D wPoint = transform->matrix() * QVector3D(0.f, 0.f, 0.f);
    const QVector3D wNorm = transform->matrix() * QVector3D(0.f, 1.0f, 0.f) - wPoint;
    const QVector3D mul = wNorm * wPoint;
    const QVector4D plVec(wNorm.x(), wNorm.y(), wNorm.z(), - mul.x() - mul.y() - mul.z());

    auto matVec = d_ptr->model->componentsOfType <QMaterial> ();
    Q_ASSERT(matVec.size() > 0);
    foreach(QParameter * const param, matVec.front()->effect()->parameters())
    {
        if (param->name() == QString("plane"))
            param->setValue(plVec);
    }
}

void MainWindow::itemsSelectionChanged()
{
    ui->tvObjectInspector->model()->deleteLater();
    ui->tvObjectInspector->setModel(nullptr);

    if (d_ptr->selectedItems.size() == 1)
    {
        QAbstractItemModel * const itemModel =
                CObjInspectorFactoryMethod::createModel(*d_ptr->selectedItems.front());
        itemModel->setParent(this);
        ui->tvObjectInspector->setModel(itemModel);
        QAbstractItemDelegate * const delegate =
                CObjInspectorFactoryMethod::createDelegate(*d_ptr->selectedItems.front());
        delegate->setParent(this);
        ui->tvObjectInspector->setItemDelegateForColumn(1, delegate);
        ui->tvObjectInspector->expandAll();
    }
}

