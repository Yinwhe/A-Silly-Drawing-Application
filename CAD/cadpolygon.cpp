#include "cadpolygon.h"

CADPolygon::CADPolygon(enum::Type type)
{
    ItemType=type;
    penColor=DefaultPenColor;
    penSize=DefaultPenSize;
    penBrush=DefaultBrush;

    setMyPen(penColor, penSize);
    setMyBrush(penBrush);

    CntPoint=0;
    CnTCR=0;
    constructed=false;
    Control=false;
    Scale=1;

    QGraphicsPolygonItem::setFlag(QGraphicsPolygonItem::ItemIsSelectable,true);
    QGraphicsPolygonItem::setAcceptHoverEvents(true);

    AllItems.append(this);
}

void CADPolygon::setMyPen(QColor color, int size){
    setPen(QPen(color, size));
}

void CADPolygon::setMyBrush(QBrush B){
    setBrush(B);
}

void CADPolygon::addPoint(QPointF p){
    if(isConstructed()){
        CntPoint--;
        ControlInit();
    }
    else if(CntPoint==0){
        ItemPoints[0]=ItemPoints[1]=p;
        CntPoint+=2;
    }else{
        ItemPoints[CntPoint-1]=ItemPoints[CntPoint]=p;
        CntPoint++;
    }
    form();
}

void CADPolygon::addTempPoint(QPointF p){
    ItemPoints[CntPoint-1]=p;
    form();
}

bool CADPolygon::isDone(){
    if(CntPoint>=30) return true;
    return false;
}

void CADPolygon::form(){
    QPolygonF tmp;
    for(int i=0;i<CntPoint;i++)
        tmp.append(ItemPoints[i]);
    setPolygon(tmp);
}

QGraphicsItem* CADPolygon::getItem(){
    return this;
};

bool CADPolygon::isConstructed(){
    return constructed;
}

void CADPolygon::setConstructed(bool c){
    constructed=c;
}

void CADPolygon::hoverEnterEvent(QGraphicsSceneHoverEvent *event){
    Q_UNUSED(event);
    this->setPen(QPen(Qt::green, penSize+1));
}

void CADPolygon::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){
    Q_UNUSED(event);
    if(selectedItem!=this) this->setPen(QPen(penColor, penSize));
}

void CADPolygon::mousePressEvent(QGraphicsSceneMouseEvent *event){
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

void CADPolygon::ControlInit(){
    CnTCR = CntPoint;
    for(int i=0;i<CntPoint;i++)
        CR[i] = new ControlRect(i, ItemPoints[i], this, Itemedge);
    CR[CnTCR] = new ControlRect(CnTCR, getCenter(), this, Itemcenter);
    CR[CnTCR+1] = new ControlRect(CnTCR+1,CR[CnTCR]->pos()+getVertical(ItemPoints[0], ItemPoints[1], 0.25*getLength(ItemPoints[0], ItemPoints[1])),this, Itemmatrix);
    handle[0] = new QGraphicsLineItem(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos()), this);
    updateLength();
    updateAngle();
}

void CADPolygon::ControlItem(bool c){
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

void CADPolygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    if(isConstructed()) ControlItem(Control);
    QGraphicsPolygonItem::paint(painter,option,widget);
}

void CADPolygon::updateOriPoints(){
    for(int i=0;i<CntPoint;i++)
        ItemPoints[i]=CR[i]->pos();
}

void CADPolygon::updateOriPoint(int index, QPointF Offset){
    ItemPoints[index]+=Offset;
}
void CADPolygon::updateCenterPoint(){
    CR[CnTCR]->setPos(getCenter());
    CR[CnTCR+1]->setPos(CR[CnTCR]->pos()+getVertical(ItemPoints[0], ItemPoints[1], Scale*0.25*getLength(ItemPoints[0], ItemPoints[1])));
    handle[0]->setLine(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos()));
}
void CADPolygon::updateLength(){
    for(int i=0;i<=CnTCR+1;i++)
        initLen[i]=getLength(CR[i]->pos(), CR[CnTCR]->pos());
}
void CADPolygon::updateAngle(){
    lastAngle=qAcos((CR[CnTCR+1]->x()-CR[CnTCR]->x())/(getLength(CR[CnTCR+1]->pos(), CR[CnTCR]->pos())));
}
QPointF CADPolygon::getCenter(){
    return boundingRect().center();
}

qreal CADPolygon::getLength(QPointF p1, QPointF p2){
    return qSqrt(qPow(p2.x()-p1.x(), 2)+qPow(p2.y()-p1.y(), 2));
}

QPointF CADPolygon::getVertical(QPointF p1, QPointF p2, qreal factor){
    QPointF tmp(p2.y()-p1.y(), p1.x()-p2.x());
    return factor*tmp/getLength(p1, p2);
}

void CADPolygon::translate(QPointF Offset){
    for(int i=0;i<CnTCR;i++)
        CR[i]->setPos(CR[i]->pos()+Offset);
    updateOriPoints();
    CR[CnTCR+1]->setPos(CR[CnTCR+1]->pos()+Offset);
}
void CADPolygon::rotate(QPointF current){
    qreal sangle=qAcos((CR[CnTCR+1]->x()-CR[CnTCR]->x())/(getLength(current, CR[CnTCR]->pos())));
    qreal angle=(current.y()<CR[CnTCR]->y())?lastAngle-sangle:sangle-lastAngle;
    lastAngle=sangle;
    for(int i=0;i<CnTCR;i++){
        CR[i]->setPos((CR[i]->x()-CR[CnTCR]->x())*qCos(angle)-(CR[i]->y()-CR[CnTCR]->y())*qSin(angle)+CR[CnTCR]->x(),
                      (CR[i]->x()-CR[CnTCR]->x())*qSin(angle)+(CR[i]->y()-CR[CnTCR]->y())*qCos(angle)+CR[CnTCR]->y());
    }
    updateOriPoints();
}
void CADPolygon::scale(QPointF p){
    Scale=getLength(p, CR[CnTCR]->pos())/initLen[CnTCR+1];
    for(int i=0;i<CnTCR;i++)
        CR[i]->setPos(releventToCenter(Scale*initLen[i]*(CR[i]->pos()-CR[CnTCR]->pos())/getLength(CR[i]->pos(), CR[CnTCR]->pos())));
    updateOriPoints();
    updateLength();
}

QPointF CADPolygon::releventToCenter(QPointF p){
    return p+CR[CnTCR]->pos();
}
int CADPolygon::getZvalue(){
    return this->zValue();
}
void CADPolygon::setZvalue(int z){
    this->setZValue(z);
}

