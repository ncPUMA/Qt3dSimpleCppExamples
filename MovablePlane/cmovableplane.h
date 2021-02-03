#ifndef CMOVABLEPLANE_H
#define CMOVABLEPLANE_H

#include <Qt3DCore/QEntity>

class CMovablePlanePrivate;

class CMovablePlane : public Qt3DCore::QEntity
{
public:
    CMovablePlane(QNode *parent = nullptr);
    ~CMovablePlane();

    void setColors(const QColor &sideA, const QColor &sideB,
                   const QColor &border, const QColor &spheres);
    void setAlpha(const float alpha);

private:
    CMovablePlanePrivate * const d_ptr;
};

#endif // CMOVABLEPLANE_H
