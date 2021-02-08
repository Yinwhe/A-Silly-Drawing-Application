#include "cadcurve.h"

CADCurve::CADCurve(enum::Type type)
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

    QGraphicsPathItem::setFlag(QGraphicsPolygonItem::ItemIsSelectable,true);
    QGraphicsPathItem::setAcceptHoverEvents(true);

    AllItems.append(this);
}

void CADCurve::setMyPen(QColor color, int size){
    setPen(QPen(color, size));
}

void CADCurve::setMyBrush(QBrush B){
    Q_UNUSED(B);
}

void CADCurve::addPoint(QPointF p){
    if(CntPoint==0){
        for(int i=0;i<4;i++) ItemPoints[i]=p;
        CntPoint++;
    }
    else if(CntPoint==1){
        ItemPoints[1]=ItemPoints[0]+(p-ItemPoints[0])/4;
        ItemPoints[2]=ItemPoints[0]+3*(p-ItemPoints[0])/4;
        ItemPoints[3]=p;
        CntPoint=4;

        constructed=true;
        ControlInit();
    }
    form();
}

void CADCurve::addTempPoint(QPointF p){
    ItemPoints[3]=p;
    form();
}

bool CADCurve::isDone(){
    return isConstructed();
}

void CADCurve::form(){
    QPainterPath path;
    path.moveTo(ItemPoints[0]);
    path.cubicTo(ItemPoints[1], ItemPoints[2], ItemPoints[3]);
    setPath(path);
}

QGraphicsItem* CADCurve::getItem(){
    return this;
};

bool CADCurve::isConstructed(){
    return constructed;
}

void CADCurve::setConstructed(bool c){
    constructed=c;
}

void CADCurve::hoverEnterEvent(QGraphicsSceneHoverEvent *event){
    Q_UNUSED(event);
    this->setPen(QPen(Qt::green, penSize+1));
}

void CADCurve::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){
    Q_UNUSED(event);
    if(selectedItem!=this) this->setPen(QPen(penColor, penSize));
}

void CADCurve::mousePressEvent(QGraphicsSceneMouseEvent *event){
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

void CADCurve::ControlInit(){
    for(int i=0;i<CnTCR;i++)
        CR[i] = new ControlRect(i, ItemPoints[i], this, Itemedge);

    CR[CnTCR] = new ControlRect(CnTCR, (ItemPoints[0]+ItemPoints[3])/2, this, Itemcenter);
    CR[CnTCR+1] = new ControlRect(CnTCR+1,CR[CnTCR]->pos()+getVertical(ItemPoints[0], ItemPoints[3], 0.25*getLength(ItemPoints[0], ItemPoints[3])),this, Itemmatrix);
    handle[0] = new QGraphicsLineItem(QLineF(CR[0]->pos(), CR[1]->pos()),this);
    handle[1] = new QGraphicsLineItem(QLineF(CR[2]->pos(), CR[3]->pos()),this);
    handle[2] = new QGraphicsLineItem(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos()), this);
    updateLength();
    updateAngle();
}

void CADCurve::ControlItem(bool c){
    if(c){
        for(int i=0;i<=CnTCR+1;i++)
            CR[i]->setVisible(true);
        handle[0]->setLine(QLineF(CR[0]->pos(), CR[1]->pos())), handle[0]->setVisible(true);
        handle[1]->setLine(QLineF(CR[2]->pos(), CR[3]->pos())), handle[1]->setVisible(true);
        handle[2]->setLine(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos())), handle[2]->setVisible(true);
    }
    else{
        for(int i=0;i<=CnTCR+1;i++)
            CR[i]->setVisible(false);
        for(int i=0;i<=2;i++)
            handle[i]->setVisible(false);
    }
    form();
}

void CADCurve::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    if(isConstructed()) ControlItem(Control);
    QGraphicsPathItem::paint(painter,option,widget);
}

void CADCurve::updateOriPoints(){
    for(int i=0;i<CntPoint;i++)
        ItemPoints[i]=CR[i]->pos();
}
void CADCurve::updateOriPoint(int index, QPointF Offset){
    ItemPoints[index]+=Offset;
}

void CADCurve::updateCenterPoint(){
    CR[CnTCR]->setPos((CR[0]->pos()+CR[3]->pos())/2);
    CR[CnTCR+1]->setPos(CR[CnTCR]->pos()+getVertical(ItemPoints[0], ItemPoints[3], Scale*0.25*getLength(ItemPoints[0], ItemPoints[3])));
    handle[2]->setLine(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos()));
}
void CADCurve::updateLength(){
    for(int i=0;i<=CnTCR+1;i++)
        initLen[i]=getLength(CR[i]->pos(), CR[CnTCR]->pos());
}
void CADCurve::updateAngle(){
    lastAngle=qAcos((CR[CnTCR+1]->x()-CR[CnTCR]->x())/(getLength(CR[CnTCR+1]->pos(), CR[CnTCR]->pos())));
}
QPointF CADCurve::getCenter(){
    return boundingRect().center();
}

qreal CADCurve::getLength(QPointF p1, QPointF p2){
    return qSqrt(qPow(p2.x()-p1.x(), 2)+qPow(p2.y()-p1.y(), 2));
}

QPointF CADCurve::getVertical(QPointF p1, QPointF p2, qreal factor){
    QPointF tmp(p2.y()-p1.y(), p1.x()-p2.x());
    return factor*tmp/getLength(p1, p2);
}

void CADCurve::translate(QPointF Offset){
    for(int i=0;i<CnTCR;i++)
        CR[i]->setPos(CR[i]->pos()+Offset);
    updateOriPoints();
    updateCenterPoint();
}
void CADCurve::rotate(QPointF current){
    qreal sangle=qAcos((CR[CnTCR+1]->x()-CR[CnTCR]->x())/(getLength(current, CR[CnTCR]->pos())));
    qreal angle=(current.y()<CR[CnTCR]->y())?lastAngle-sangle:sangle-lastAngle;
    lastAngle=sangle;
    for(int i=0;i<CnTCR;i++){
        CR[i]->setPos((CR[i]->x()-CR[CnTCR]->x())*qCos(angle)-(CR[i]->y()-CR[CnTCR]->y())*qSin(angle)+CR[CnTCR]->x(),
                      (CR[i]->x()-CR[CnTCR]->x())*qSin(angle)+(CR[i]->y()-CR[CnTCR]->y())*qCos(angle)+CR[CnTCR]->y());
    }
    updateOriPoints();
}
void CADCurve::scale(QPointF p){
    Scale=getLength(p, CR[CnTCR]->pos())/initLen[CnTCR+1];
    for(int i=0;i<CnTCR;i++)
        CR[i]->setPos(releventToCenter(Scale*initLen[i]*(CR[i]->pos()-CR[CnTCR]->pos())/getLength(CR[i]->pos(), CR[CnTCR]->pos())));
    updateOriPoints();
    updateLength();
}

QPointF CADCurve::releventToCenter(QPointF p){
    return p+CR[CnTCR]->pos();
}

int CADCurve::getZvalue(){
    return this->zValue();
}
void CADCurve::setZvalue(int z){
    this->setZValue(z);
}
