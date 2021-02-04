#ifndef CMOVABLEPLANE_H
#define CMOVABLEPLANE_H

#include <Qt3DCore/QEntity>

namespace Qt3DRender {
    class QPickEvent;
    class QObjectPicker;
}

class CMovablePlanePrivate;

class CMovablePlane : public Qt3DCore::QEntity
{
    Q_OBJECT

public:
    CMovablePlane(QNode *parent = nullptr);
    ~CMovablePlane();

    QVector3D getPos() const;
    QSizeF getSize() const;

    void setPos(const QVector3D &pos);
    void setSize(const QSizeF &size);
    void setColors(const QColor &sideA, const QColor &sideB,
                   const QColor &border, const QColor &cubes);
    void setAlpha(const float alpha);

signals:
    void sigMouseHovered(bool containsMouse);

private slots:
    void slContainsMouseChanged(bool containsMouse);

private:
    CMovablePlanePrivate * const d_ptr;
};

#endif // CMOVABLEPLANE_H
