#ifndef CADCIRCLE_H
#define CADCIRCLE_H
#include "cadellipse.h"

class CADCircle:public CADEllipse
{
public:
    CADCircle(enum::Type);
    void addPoint(QPointF p);
    void addTempPoint(QPointF p);
    void form();

    void ControlInit();
    void ControlItem(bool);

    void updateOriPoints();
    void updateOriPoint(int index, QPointF Offset);
    void updateCenterPoint();
    void updateLength();

    void translate(QPointF Offset);
    void rotate(QPointF);
    void scale(QPointF);
};

#endif // CADCIRCLE_H
