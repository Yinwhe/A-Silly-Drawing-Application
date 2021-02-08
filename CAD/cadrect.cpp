#include "cadrect.h"

CADRect::CADRect(enum::Type type)
{
    ItemType=type;
    penColor=DefaultPenColor;
    penSize=DefaultPenSize;
    penBrush=DefaultBrush;

    setMyPen(penColor, penSize);
    setMyBrush(penBrush);

    CntPoint=0;
    CnTCR=4;
    constructed=false;
    Control=false;
    Scale=1;

    QGraphicsPolygonItem::setFlag(QGraphicsPolygonItem::ItemIsSelectable,true);
    QGraphicsPolygonItem::setAcceptHoverEvents(true);

    AllItems.append(this);
}

void CADRect::setMyPen(QColor color, int size){
    setPen(QPen(color, size));
}

void CADRect::setMyBrush(QBrush B){
    setBrush(B);
}


void CADRect::addPoint(QPointF p){
    if(CntPoint==0){
        ItemPoints[0]=ItemPoints[1]=ItemPoints[2]=ItemPoints[3]=p;
        CntPoint=4;
    }
    else{
        ItemPoints[2]=p;
        ItemPoints[1].setY(p.y());
        ItemPoints[3].setX(p.x());

        if(ItemPoints[0]==ItemPoints[2]){
            delete this;
            AllItems.pop_back();
            return;
        }

        constructed=true;
        ControlInit();
    }
    form();
}

void CADRect::addTempPoint(QPointF p){
    ItemPoints[2]=p;
    ItemPoints[1].setY(p.y());
    ItemPoints[3].setX(p.x());
    form();
}

bool CADRect::isDone(){
    if(CntPoint==4) return true;
    return false;
}

void CADRect::form(){
    QPolygonF tmp;
    for(int i=0;i<CntPoint;i++)
        tmp.append(ItemPoints[i]);
    setPolygon(tmp);
}

QGraphicsItem* CADRect::getItem(){
    return this;
};

bool CADRect::isConstructed(){
    return constructed;
}

void CADRect::setConstructed(bool c){
    constructed=c;
}

void CADRect::hoverEnterEvent(QGraphicsSceneHoverEvent *event){
    Q_UNUSED(event);
    this->setPen(QPen(Qt::green, penSize+1));
}

void CADRect::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){
    Q_UNUSED(event);
    if(selectedItem!=this) this->setPen(QPen(penColor, penSize));
}

void CADRect::mousePressEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event);
    if(!selectedItem){
        selectedItem=this;
        selectedItem->setMyPen(Qt::green, penSize+1);
        selectedItem->Control=true;
    }
    else if(selectedItem!=this){
        selectedItem->setMyPen(selectedItem->penColor,selectedItem->penSize);
        selectedItem->Control=false;
        selectedItem=this;
        selectedItem->setMyPen(Qt::green, penSize+1);
        selectedItem->Control=true;
    }
    else{
        selectedItem->setMyPen(selectedItem->penColor,selectedItem->penSize);
        selectedItem->Control=false;
        selectedItem=nullptr;
    }
    form();
}
/*
 * 0    3    3
 *
 * 0    4    2
 *
 * 1    1    2
*/
void CADRect::ControlInit(){
    for(int i=0;i<CnTCR;i++)
        CR[i] = new ControlRect(i, (ItemPoints[i]+ItemPoints[(i+1)%CnTCR])/2, this, Itemedge);
    CR[CnTCR] = new ControlRect(CnTCR, getCenter(), this, Itemcenter);
    CR[CnTCR+1] = new ControlRect(CnTCR+1,CR[CnTCR]->pos()+getVertical(ItemPoints[0], ItemPoints[1], 0.25*getLength(ItemPoints[0], ItemPoints[1])),this, Itemmatrix);
    handle[0] = new QGraphicsLineItem(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos()), this);
    updateLength();
    updateAngle();
}

void CADRect::ControlItem(bool c){
    if(c){
        for(int i=0;i<=CnTCR+1;i++)
            CR[i]->setVisible(true);
        handle[0]->setLine(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos())), handle[0]->setVisible(true);
    }
    else{
        for(int i=0;i<=CnTCR+1;i++)
            CR[i]->setVisible(false);
        handle[0]->setVisible(false);
    }
    form();
}

void CADRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    if(isConstructed()) ControlItem(Control);
    QGraphicsPolygonItem::paint(painter,option,widget);
}
void CADRect::updateControlRect(){
    for(int i=0;i<CnTCR;i++)
        CR[i]->setPos((ItemPoints[i]+ItemPoints[(i+1)%CnTCR])/2);
}
void CADRect::updateOriPoints(){
}

void CADRect::updateOriPoint(int index, QPointF Offset){
    ItemPoints[index]+=Offset;
    ItemPoints[(index+1)%CntPoint]+=Offset;
    updateControlRect();
}
void CADRect::updateCenterPoint(){
    CR[CnTCR]->setPos(getCenter());
    CR[CnTCR+1]->setPos(CR[CnTCR]->pos()+getVertical(ItemPoints[0], ItemPoints[1], Scale*0.25*getLength(ItemPoints[0], ItemPoints[1])));
    handle[0]->setLine(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos()));
    qDebug()<<CR[CnTCR]->pos()<<' '<<CR[CnTCR+1]->pos();
}
void CADRect::updateLength(){
    for(int i=0;i<CntPoint;i++)
        initLen[i]=getLength(ItemPoints[i], CR[CnTCR]->pos());
    initLen[CnTCR+1]=getLength(CR[CnTCR+1]->pos(), CR[CnTCR]->pos());
}
void CADRect::updateAngle(){
    lastAngle=qAcos((CR[CnTCR+1]->x()-CR[CnTCR]->x())/(getLength(CR[CnTCR+1]->pos(), CR[CnTCR]->pos())));
}
QPointF CADRect::getCenter(){
    return (ItemPoints[0]+ItemPoints[2])/2;
}

qreal CADRect::getLength(QPointF p1, QPointF p2){
    return qSqrt(qPow(p2.x()-p1.x(), 2)+qPow(p2.y()-p1.y(), 2));
}

QPointF CADRect::getVertical(QPointF p1, QPointF p2, qreal factor){
    QPointF tmp(p2.y()-p1.y(), p1.x()-p2.x());
    return factor*tmp/getLength(p1, p2);
}

void CADRect::translate(QPointF Offset){
    for(int i=0;i<CntPoint;i++)
        ItemPoints[i]+=Offset;
    updateControlRect();
    CR[CnTCR+1]->setPos(CR[CnTCR+1]->pos()+Offset);
}
void CADRect::rotate(QPointF current){
    qreal sangle=qAcos((CR[CnTCR+1]->x()-CR[CnTCR]->x())/(getLength(current, CR[CnTCR]->pos())));
    qreal angle=(current.y()<CR[CnTCR]->y())?lastAngle-sangle:sangle-lastAngle;
    lastAngle=sangle;
    for(int i=0;i<CntPoint;i++){
        ItemPoints[i]=QPointF((ItemPoints[i].x()-CR[CnTCR]->x())*qCos(angle)-(ItemPoints[i].y()-CR[CnTCR]->y())*qSin(angle)+CR[CnTCR]->x(),
                      (ItemPoints[i].x()-CR[CnTCR]->x())*qSin(angle)+(ItemPoints[i].y()-CR[CnTCR]->y())*qCos(angle)+CR[CnTCR]->y());
    }
    updateControlRect();
}
void CADRect::scale(QPointF p){
    Scale=getLength(p, CR[CnTCR]->pos())/initLen[CnTCR+1];
    for(int i=0;i<CntPoint;i++)
        ItemPoints[i]=QPointF(releventToCenter(Scale*initLen[i]*(ItemPoints[i]-CR[CnTCR]->pos())/getLength(ItemPoints[i], CR[CnTCR]->pos())));
    updateControlRect();
    updateLength();
}

QPointF CADRect::releventToCenter(QPointF p){
    return p+CR[CnTCR]->pos();
}
int CADRect::getZvalue(){
    return this->zValue();
}
void CADRect::setZvalue(int z){
    this->setZValue(z);
}

