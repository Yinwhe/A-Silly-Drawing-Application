#ifndef CADLINE_H
#define CADLINE_H
#include "baseitem.h"
#include "controlrect.h"
#include <QGraphicsLineItem>
class CADLine:public BaseItem, public QGraphicsLineItem
{
public:
    CADLine(enum::Type type);
    void addPoint(QPointF p);
    void addTempPoint(QPointF p);
    bool isDone();
    void form();
    QGraphicsItem* getItem();
    void setMyPen(QColor, int);
    void setMyBrush(QBrush);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void ControlInit();
    void ControlItem(bool);

    void updateOriPoints();
    void updateOriPoint(int index, QPointF Offset);
    void updateCenterPoint();
    void updateLength();
    void updateAngle();

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    void setConstructed(bool);
    bool isConstructed();

    QPointF getCenter();
    QPointF getVertical(QPointF, QPointF, qreal);
    qreal getLength(QPointF, QPointF);
    QPointF releventToCenter(QPointF);

    void translate(QPointF Offset);
    void rotate(QPointF);
    void scale(QPointF);

    int getZvalue();
    void setZvalue(int);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // CADLINE_H
