#ifndef BASEITEM_H
#define BASEITEM_H
#include <QGraphicsItem>
#include <QPoint>
#include <QPen>
#include <QBrush>
#include <QtDebug>
#include <QtMath>
#include <QLabel>
#define MAXNODE 35
//MAXNODE is the maxmum number of nodes that a graph can have, it's changeable.
class ControlRect;

enum Mode{justcursor,createnew, draw};
enum Type{line, curve, rectangle, circle, ellipse, polygon};

//this is the base class of all other graph
class BaseItem
{
public:
    //about create a graph
    virtual void addPoint(QPointF p)=0;
    virtual void addTempPoint(QPointF p)=0;
    virtual void form()=0;

    //whether a graph is completed or not
    virtual bool isDone()=0;
    virtual void setConstructed(bool)=0;
    virtual bool isConstructed()=0;

    //get the graph
    virtual QGraphicsItem* getItem()=0;

    //change status of pen
    virtual void setMyPen(QColor, int)=0;
    virtual void setMyBrush(QBrush)=0;

    //some mouse event
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event)=0;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event)=0;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event)=0;

    //control the items
    virtual void ControlInit()=0;
    virtual void ControlItem(bool)=0;

    //update relevent
    virtual void updateOriPoints()=0;
    virtual void updateOriPoint(int index, QPointF Offset)=0;
    virtual void updateCenterPoint()=0;
    virtual void updateLength()=0;
    virtual void updateAngle()=0;

    //position of point relevent
    virtual QPointF getCenter()=0;
    virtual QPointF getVertical(QPointF, QPointF, qreal factor)=0;
    virtual qreal getLength(QPointF, QPointF)=0;
    virtual QPointF releventToCenter(QPointF)=0;

    //matrix relevent
    virtual void translate(QPointF Offset)=0;
    virtual void rotate(QPointF)=0;
    virtual void scale(QPointF)=0;

    //graph layer relevent
    virtual int getZvalue()=0;
    virtual void setZvalue(int)=0;

public:
    QColor penColor;
    int penSize;
    QBrush penBrush;
    enum::Type ItemType;
    int CntPoint;
    int CnTCR;
    bool Control;
    QPointF ItemPoints[MAXNODE];

protected:
    bool constructed;
    qreal Scale;
    qreal initLen[MAXNODE];
    qreal lastAngle;
    QPointF TempPoints[MAXNODE];
    ControlRect* CR[MAXNODE+2];
    QGraphicsLineItem *handle[MAXNODE];
};

extern BaseItem* selectedItem;
extern QColor DefaultPenColor;
extern int DefaultPenSize;
extern QBrush DefaultBrush;
extern QList<BaseItem*> AllItems;
extern QList<BaseItem*> Trash;
#endif // BASEITEM_H
