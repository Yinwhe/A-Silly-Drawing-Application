#include "cadline.h"
CADLine::CADLine(enum::Type type)
{
    ItemType=type;
    penColor=DefaultPenColor;
    penSize=DefaultPenSize;
    penBrush=DefaultBrush;

    setMyPen(penColor, penSize);
    setMyBrush(penBrush);

    CntPoint=0;
    CnTCR=2;
    constructed=false;
    Control=false;
    Scale=1;

    QGraphicsLineItem::setFlag(QGraphicsPolygonItem::ItemIsSelectable,true);
    QGraphicsLineItem::setAcceptHoverEvents(true);

    AllItems.append(this);
}

void CADLine::setMyPen(QColor color, int size){
    setPen(QPen(color, size));
}

void CADLine::setMyBrush(QBrush B){
    Q_UNUSED(B);
}

void CADLine::addPoint(QPointF p){
    if(CntPoint==0){
        ItemPoints[0]=ItemPoints[1]=p;
        CntPoint++;
    }
    else if(CntPoint==1){
        ItemPoints[1]=p;
        CntPoint++;

        if(ItemPoints[0]==ItemPoints[1]){
            delete this;
            AllItems.pop_back();
            return;
        }

        constructed=true;
        ControlInit();
    }
    form();
}
void CADLine::addTempPoint(QPointF p){
    ItemPoints[1]=p;
    form();
}

bool CADLine::isDone(){
    if(CntPoint==2)return true;
    return false;
}

void CADLine::form(){
    setLine(QLineF(ItemPoints[0], ItemPoints[1]));
}
QGraphicsItem* CADLine::getItem(){
    return this;
};

void CADLine::setConstructed(bool c){
    constructed=c;
}

bool CADLine::isConstructed(){
    return constructed;
}

void CADLine::hoverEnterEvent(QGraphicsSceneHoverEvent *event){
    Q_UNUSED(event);
    this->setPen(QPen(Qt::green, penSize+1));
}

void CADLine::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){
    Q_UNUSED(event);
    if(selectedItem!=this) this->setPen(QPen(penColor, penSize));
}

void CADLine::mousePressEvent(QGraphicsSceneMouseEvent *event){
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

void CADLine::ControlInit(){
    for(int i=0;i<2;i++)
        CR[i] = new ControlRect(i, ItemPoints[i], this, Itemedge);
    CR[CnTCR] = new ControlRect(CnTCR, (ItemPoints[0]+ItemPoints[1])/2, this, Itemcenter);
    CR[CnTCR+1] = new ControlRect(CnTCR+1,CR[CnTCR]->pos()+getVertical(ItemPoints[0], ItemPoints[1], 0.25*getLength(ItemPoints[0], ItemPoints[1])),this, Itemmatrix);
    handle[0] = new QGraphicsLineItem(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos()), this);
    updateLength();
    updateAngle();
}

void CADLine::ControlItem(bool c){
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

void CADLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    if(isConstructed()) ControlItem(Control);
    QGraphicsLineItem::paint(painter,option,widget);
}

void CADLine::updateOriPoints(){
    for(int i=0;i<CntPoint;i++)
        ItemPoints[i]=CR[i]->pos();
}
void CADLine::updateOriPoint(int index, QPointF Offset){
    ItemPoints[index]+=Offset;
}
void CADLine::updateCenterPoint(){
    CR[CnTCR]->setPos((CR[0]->pos()+CR[1]->pos())/2);
    CR[CnTCR+1]->setPos(CR[CnTCR]->pos()+getVertical(ItemPoints[0], ItemPoints[1], Scale*0.25*getLength(ItemPoints[0], ItemPoints[1])));
    handle[0]->setLine(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos()));
}
void CADLine::updateLength(){
    for(int i=0;i<=CnTCR+1;i++)
        initLen[i]=getLength(CR[i]->pos(), CR[CnTCR]->pos());
}
void CADLine::updateAngle(){
    lastAngle=qAcos((CR[CnTCR+1]->x()-CR[CnTCR]->x())/(getLength(CR[CnTCR+1]->pos(), CR[CnTCR]->pos())));
}
QPointF CADLine::getCenter(){
    return boundingRect().center();
}

qreal CADLine::getLength(QPointF p1, QPointF p2){
    return qSqrt(qPow(p2.x()-p1.x(), 2)+qPow(p2.y()-p1.y(), 2));
}

QPointF CADLine::getVertical(QPointF p1, QPointF p2, qreal factor){
    QPointF tmp(p2.y()-p1.y(), p1.x()-p2.x());
    return factor*tmp/getLength(p1, p2);
}

void CADLine::translate(QPointF Offset){
    for(int i=0;i<CnTCR;i++)
        CR[i]->setPos(CR[i]->pos()+Offset);
    updateOriPoints();
    updateCenterPoint();
}
void CADLine::rotate(QPointF current){
    qreal sangle=qAcos((CR[CnTCR+1]->x()-CR[CnTCR]->x())/(getLength(current, CR[CnTCR]->pos())));
    qreal angle=(current.y()<CR[CnTCR]->y())?lastAngle-sangle:sangle-lastAngle;
    lastAngle=sangle;
    for(int i=0;i<CnTCR;i++){
        CR[i]->setPos((CR[i]->x()-CR[CnTCR]->x())*qCos(angle)-(CR[i]->y()-CR[CnTCR]->y())*qSin(angle)+CR[CnTCR]->x(),
                      (CR[i]->x()-CR[CnTCR]->x())*qSin(angle)+(CR[i]->y()-CR[CnTCR]->y())*qCos(angle)+CR[CnTCR]->y());
    }
    updateOriPoints();
}
void CADLine::scale(QPointF p){
    Scale=getLength(p, CR[CnTCR]->pos())/initLen[CnTCR+1];
    for(int i=0;i<CnTCR;i++)
        CR[i]->setPos(releventToCenter(Scale*initLen[i]*(CR[i]->pos()-CR[CnTCR]->pos())/getLength(CR[i]->pos(), CR[CnTCR]->pos())));
    updateOriPoints();
    updateLength();
}

QPointF CADLine::releventToCenter(QPointF p){
    return p+CR[CnTCR]->pos();
}
int CADLine::getZvalue(){
    return this->zValue();
}
void CADLine::setZvalue(int z){
    this->setZValue(z);
}
