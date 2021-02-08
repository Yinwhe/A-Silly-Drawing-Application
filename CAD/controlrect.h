#ifndef CONTROLRECT_H
#define CONTROLRECT_H
#include "baseitem.h"
#include <QGraphicsRectItem>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QtDebug>

enum ControlRectType{Itemedge, Itemcenter, Itemmatrix};

class ControlRect:public QGraphicsRectItem
{
public:
    ControlRect(int index, const QPointF &point, BaseItem *parent, ControlRectType type);

    void setPoint(const QPointF & point);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    BaseItem * parentItem;
    int controlRectIndex;
    ControlRectType ControlType;

    QPointF tmpPos;
};

#endif // CONTROLRECT_H
