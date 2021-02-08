#include "cadcircle.h"

CADCircle::CADCircle(enum::Type type):CADEllipse(type){
    CnTCR=0;
}

/*
 *2
 *   0
 *      1
*/

void CADCircle::addPoint(QPointF p){
    if(CntPoint==0){
        ItemPoints[0]=ItemPoints[1]=ItemPoints[2]=p;
        CntPoint++;
    }
    else if(CntPoint==1){
        qreal r=getLength(p, ItemPoints[0])/(sqrt(2));
        ItemPoints[1]=ItemPoints[0]+QPointF(r,r);
        ItemPoints[2]=ItemPoints[0]-QPointF(r,r);
        CntPoint=3;

        if(ItemPoints[1]==ItemPoints[2]){
            delete this;
            AllItems.pop_back();
            return;
        }

        constructed=true;
        ControlInit();
    }
    form();
}
void CADCircle::addTempPoint(QPointF p){
    qreal r=getLength(p, ItemPoints[0])/(sqrt(2));
    ItemPoints[1]=ItemPoints[0]+QPointF(r,r);
    ItemPoints[2]=ItemPoints[0]-QPointF(r,r);
    form();
}

void CADCircle::form(){
    TempPoints[1]=ItemPoints[1], TempPoints[2]=ItemPoints[2];
    if(TempPoints[2].x()>TempPoints[1].x()){
        TempPoints[2].setX(TempPoints[1].x());
        TempPoints[1].setX(ItemPoints[2].x());
    }
    if(TempPoints[2].y()>TempPoints[1].y()){
        TempPoints[2].setY(TempPoints[1].y());
        TempPoints[1].setY(ItemPoints[2].y());
    }
    setRect(QRectF(TempPoints[2], TempPoints[1]));
}

void CADCircle::ControlInit(){
    CR[CnTCR] = new ControlRect(CnTCR, ItemPoints[0], this, Itemcenter);
    CR[CnTCR+1] = new ControlRect(CnTCR+1, ItemPoints[0]+QPointF((ItemPoints[1]-ItemPoints[0]).x(),0),this, Itemmatrix);
    handle[0] = new QGraphicsLineItem(QLineF(CR[CnTCR]->pos(), CR[CnTCR+1]->pos()), this);
    updateLength();
    updateAngle();
}
void CADCircle::ControlItem(bool c){
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

void CADCircle::updateOriPoints(){
    qreal len = getLength(CR[CnTCR+1]->pos(), CR[CnTCR]->pos())-initLen[CnTCR+1];
    ItemPoints[1]=ItemPoints[0]+QPointF(len, len);
    ItemPoints[2]=ItemPoints[0]-QPointF(len,len);
    updateLength();
}
void CADCircle::updateOriPoint(int index, QPointF Offset){
    Q_UNUSED(index);
    Q_UNUSED(Offset);
    updateOriPoints();
}

void CADCircle::updateCenterPoint(){
    CR[CnTCR]->setPos(getCenter());
}
//initLen[0] is r
void CADCircle::updateLength(){
    initLen[0]=getLength(CR[CnTCR+1]->pos(), CR[CnTCR]->pos());
    initLen[1]=getLength(ItemPoints[1], ItemPoints[0]);
    initLen[2]=getLength(ItemPoints[2], ItemPoints[0]);
}

void CADCircle::translate(QPointF Offset){
    for(int i=0;i<=CntPoint;i++)
        ItemPoints[i]+=Offset;
    CR[CnTCR+1]->setPos(CR[CnTCR+1]->pos()+Offset);
}

void CADCircle::rotate(QPointF current){
    Q_UNUSED(current);
}
void CADCircle::scale(QPointF p){
    Scale=getLength(p, CR[CnTCR]->pos())/initLen[0];
    ItemPoints[1] = QPointF(releventToCenter(Scale*initLen[1]*(ItemPoints[1]-CR[CnTCR]->pos())/getLength(ItemPoints[1], CR[CnTCR]->pos())));
    ItemPoints[2] = QPointF(releventToCenter(Scale*initLen[2]*(ItemPoints[2]-CR[CnTCR]->pos())/getLength(ItemPoints[2], CR[CnTCR]->pos())));
    updateLength();
}
