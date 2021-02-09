#ifndef CMOVABLEPLANE_H
#define CMOVABLEPLANE_H

#include <Qt3DCore/QEntity>

namespace Qt3DRender {
    class QPickEvent;
}

class CMovablePlanePrivate;

namespace MovablePlane
{

enum EN_TransformationMode
{
    ENTM_FIRST = 0,

    ENTM_OFF = ENTM_FIRST,
    ENTM_MOVE,
    ENTM_RESIZE,
    ENTM_ROTATE,

    ENTM_LAST
};
typedef int TTransformationMode;

}

class CMovablePlane : public Qt3DCore::QEntity
{
    Q_OBJECT

public:
    CMovablePlane(QNode *parent = nullptr);
    ~CMovablePlane();

    QString getName() const;
    QVector3D getPos() const;
    QSizeF getSize() const;

    void setName(const QString &name);
    void setTransformationMode(const MovablePlane::TTransformationMode mode);
    void setPos(const QVector3D &pos);
    void setSize(const QSizeF &size);
    void setColors(const QColor &sideA, const QColor &sideB,
                   const QColor &border, const QColor &cubes);
    void setAlpha(const float alpha);

signals:
    void sigNameChanged(QString name);
    void sigSizeChanged();
    void sigMouseHovered(bool containsMouse);

private slots:
    void slContainsMouseChanged(bool containsMouse);
    void slPressed(Qt3DRender::QPickEvent *pic);
    void slReleased(Qt3DRender::QPickEvent *pic);
    void slMoved(Qt3DRender::QPickEvent *pic);

private:
    CMovablePlanePrivate * const d_ptr;
};

#endif // CMOVABLEPLANE_H
