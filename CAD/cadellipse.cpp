#include "cadellipse.h"

CADEllipse::CADEllipse(enum::Type type)
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

    QGraphicsEllipseItem::setFlag(QGraphicsPolygonItem::ItemIsSelectable,true);
    QGraphicsEllipseItem::setAcceptHoverEvents(true);

    AllItems.append(this);
}


void CADEllipse::setMyPen(QColor color, int size){
    setPen(QPen(color, size));
}

void CADEllipse::setMyBrush(QBrush B){
    setBrush(B);
}

void CADEllipse::addPoint(QPointF p){
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

void CADEllipse::addTempPoint(QPointF p){
    ItemPoints[1]=p;
    form();
}

bool CADEllipse::isDone(){
    if(CntPoint==2)return true;
    return false;
}

void CADEllipse::form(){
    TempPoints[0]=ItemPoints[0], TempPoints[1]=ItemPoints[1];
    if(TempPoints[0].x()>TempPoints[1].x()){
        TempPoints[0].setX(TempPoints[1].x());
        TempPoints[1].setX(ItemPoints[0].x());
    }
    if(TempPoints[0].y()>TempPoints[1].y()){
        TempPoints[0].setY(TempPoints[1].y());
        TempPoints[1].setY(ItemPoints[0].y());
    }

    setRect(QRectF(TempPoints[0], TempPoints[1]));
}

QGraphicsItem* CADEllipse::getItem(){
    return this;
};

void CADEllipse::setConstructed(bool c){
    constructed=c;
}

bool CADEllipse::isConstructed(){
    return constructed;
}

void CADEllipse::hoverEnterEvent(QGraphicsSceneHoverEvent *event){
    Q_UNUSED(event);
    this->setPen(QPen(Qt::green, penSize+1));
}

void CADEllipse::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){
    Q_UNUSED(event);
    if(selectedItem!=this) this->setPen(QPen(penColor, penSize));
}

void CADEllipse::mousePressEvent(QGraphicsSceneMouseEvent *event){
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
 * 0---3
 * | 4 |
 * 2---1
*/

void CADEllipse::ControlInit(){
    for(int i=0;i<2;i++){
        CR[i] = new ControlRect(i, ItemPoints[i], this, Itemedge);
    }

    CR[CnTCR] = new ControlRect(CnTCR, (ItemPoints[0]+ItemPoints[1])/2, this, Itemcenter);
    CR[CnTCR+1] = new ControlRect(CnTCR+1,CR[CnTCR]->pos()+getVertical(ItemPoints[0], ItemPoints[1], 0.25*getLength(ItemPoints[0], ItemPoints[1])),this, Itemmatrix);
    handle[0] = new QGraphicsLineItem(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos()), this);
    updateLength();
    updateAngle();
}

void CADEllipse::ControlItem(bool c){
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

void CADEllipse::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    if(isConstructed()) ControlItem(Control);
    QGraphicsEllipseItem::paint(painter,option,widget);
}

void CADEllipse::updateOriPoints(){
    for(int i=0;i<2;i++)
        ItemPoints[i]=CR[i]->pos();
}
void CADEllipse::updateOriPoint(int index, QPointF Offset){
    ItemPoints[index]+=Offset;
}

void CADEllipse::updateCenterPoint(){
    CR[CnTCR]->setPos(getCenter());
    CR[CnTCR+1]->setPos(CR[CnTCR]->pos()+getVertical(ItemPoints[0], ItemPoints[1], Scale*0.25*getLength(ItemPoints[0], ItemPoints[1])));
    handle[0]->setLine(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos()));
}
void CADEllipse::updateLength(){
    for(int i=0;i<=CnTCR+1;i++)
        initLen[i]=getLength(CR[i]->pos(), CR[CnTCR]->pos());
}

QPointF CADEllipse::getCenter(){
    return boundingRect().center();
}

qreal CADEllipse::getLength(QPointF p1, QPointF p2){
    return qSqrt(qPow(p2.x()-p1.x(), 2)+qPow(p2.y()-p1.y(), 2));
}
void CADEllipse::updateAngle(){
    lastAngle=qAcos((CR[CnTCR+1]->x()-CR[CnTCR]->x())/(getLength(CR[CnTCR+1]->pos(), CR[CnTCR]->pos())));
}
QPointF CADEllipse::getVertical(QPointF p1, QPointF p2, qreal factor){
    QPointF tmp(p2.y()-p1.y(), p1.x()-p2.x());
    return factor*tmp/getLength(p1, p2);
}

void CADEllipse::translate(QPointF Offset){
    for(int i=0;i<CnTCR;i++)
        CR[i]->setPos(CR[i]->pos()+Offset);
    updateOriPoints();
    updateCenterPoint();
}
void CADEllipse::rotate(QPointF current){
    qreal sangle=qAcos((CR[CnTCR+1]->x()-CR[CnTCR]->x())/(getLength(current, CR[CnTCR]->pos())));
    qreal angle=(current.y()<CR[CnTCR]->y())?lastAngle-sangle:sangle-lastAngle;
    lastAngle=sangle;
    for(int i=0;i<CnTCR;i++){
        CR[i]->setPos((CR[i]->x()-CR[CnTCR]->x())*qCos(angle)-(CR[i]->y()-CR[CnTCR]->y())*qSin(angle)+CR[CnTCR]->x(),
                      (CR[i]->x()-CR[CnTCR]->x())*qSin(angle)+(CR[i]->y()-CR[CnTCR]->y())*qCos(angle)+CR[CnTCR]->y());
    }
    updateOriPoints();
}
void CADEllipse::scale(QPointF p){
    Scale=getLength(p, CR[CnTCR]->pos())/initLen[CnTCR+1];
    for(int i=0;i<CnTCR;i++)
        CR[i]->setPos(releventToCenter(Scale*initLen[i]*(CR[i]->pos()-CR[CnTCR]->pos())/getLength(CR[i]->pos(), CR[CnTCR]->pos())));
    updateOriPoints();
    updateLength();
}

QPointF CADEllipse::releventToCenter(QPointF p){
    return p+CR[CnTCR]->pos();
}
int CADEllipse::getZvalue(){
    return this->zValue();
}
void CADEllipse::setZvalue(int z){
    this->setZValue(z);
}

