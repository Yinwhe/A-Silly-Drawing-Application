#include "controlrect.h"

ControlRect::ControlRect(int index, const QPointF &point, BaseItem *parent, ControlRectType type)
{
    controlRectIndex = index;

    parentItem = parent;
    ControlType=type;
    QColor color;
    if(type==Itemedge) color = Qt::gray;
    else if(type==Itemcenter) color = Qt::red;
    else color = Qt::blue;
    setBrush(QBrush(color));
    setPen(QPen(color));
    setPoint(point);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setParentItem(parent->getItem());
}

void ControlRect::setPoint(const QPointF &point)
{
    setPos(point);
    setRect(QRectF(QPointF(-3,-3),QPointF(3,3)));
}


void ControlRect::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);
}

void ControlRect::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    tmpPos = scenePos();
    QGraphicsRectItem::mouseMoveEvent(event);

    switch (this->ControlType) {
    case Itemedge:{
        selectedItem->updateOriPoint(this->controlRectIndex, this->scenePos()-tmpPos);
        selectedItem->updateCenterPoint();
        selectedItem->updateLength();
        selectedItem->updateAngle();
        break;
    }
    case Itemcenter:{
        selectedItem->translate(this->scenePos()-tmpPos);
        break;
    }
    case Itemmatrix:{
        selectedItem->scale(this->scenePos());
        selectedItem->rotate(this->pos());
        break;
    }
    }
}

void ControlRect::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);
}


