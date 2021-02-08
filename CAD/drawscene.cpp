#include "drawscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QtDebug>
#include <QFile>
#include <QMessageBox>

DrawScene::DrawScene(QObject *parent):
    QGraphicsScene(parent)
{
    tempItem=nullptr;
}

void DrawScene::setMode(Mode mode){
    currentMode=mode;
}
void DrawScene::setItemType(Type type){
    currentType=type;
}

void DrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent){
    qDebug()<<"Mode"<<currentMode<<' '<<"Type"<<currentType;
    if(mouseEvent->button()==Qt::LeftButton)
        switch(currentMode){
        case justcursor:QGraphicsScene::mousePressEvent(mouseEvent);break;
        case createnew:{
            switch(currentType){
            case line:{
                tempItem=new CADLine(line);
                tempItem->addPoint(mouseEvent->scenePos());
                addItem(tempItem->getItem());
                currentMode=draw;
                break;
            }
            case rectangle:{
                tempItem=new CADRect(rectangle);
                tempItem->addPoint(mouseEvent->scenePos());
                addItem(tempItem->getItem());
                currentMode=draw;
                break;
            }
            case curve:{
                tempItem=new CADCurve(curve);
                tempItem->addPoint(mouseEvent->scenePos());
                addItem(tempItem->getItem());
                currentMode=draw;
                break;
            }
            case circle:{
                tempItem=new CADCircle(circle);
                tempItem->addPoint(mouseEvent->scenePos());
                addItem(tempItem->getItem());
                currentMode=draw;
                break;
            }
            case ellipse:{
                tempItem=new CADEllipse(ellipse);
                tempItem->addPoint(mouseEvent->scenePos());
                addItem(tempItem->getItem());
                currentMode=draw;
                break;
            }
            case polygon:{
                tempItem= new CADPolygon(polygon);
                tempItem->addPoint(mouseEvent->scenePos());
                addItem(tempItem->getItem());
                currentMode=draw;
                break;
            }
            }
        }
        break;
        case draw:{
            switch(currentType){
            case polygon:{
                if(tempItem->isDone()){
                    tempItem->setConstructed(true);
                    currentMode=createnew;
                }
                tempItem->addPoint(mouseEvent->scenePos());
                break;
            }
            default:break;
            }
        }
        break;
        }
    else if(mouseEvent->button()==Qt::RightButton)
        switch(currentMode){
        case justcursor:QGraphicsScene::mousePressEvent(mouseEvent);break;
        case createnew:break;
        case draw:{
            switch(currentType){
            case polygon:{
                tempItem->setConstructed(true);
                currentMode=createnew;
                tempItem->addPoint(mouseEvent->scenePos());
                break;
            }
            default:break;
            }
        }
        break;
        }
}



void DrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent){
    switch(currentMode){
    case justcursor: QGraphicsScene::mouseMoveEvent(mouseEvent);break;
    case createnew:break;
    case draw:{
        switch(currentType){
        case polygon:break;
        default:tempItem->addTempPoint(mouseEvent->scenePos());break;
        }
    }
    }
}
void DrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent){
    switch (currentMode) {
    case justcursor:break;
    case createnew:break;
    case draw:{
        switch (currentType) {
        case polygon:break;
        default:{
            tempItem->addPoint(mouseEvent->scenePos());
            currentMode=createnew;
            break;
        }
        }
    }
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void DrawScene::SaveCADItems(QString filePath){
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::critical(0 , "FATAL ERROR!" , "FILE OPEN FAIL! PLEASE EXIT! :(", QMessageBox::Ok | QMessageBox::Default , 0, 0 );
        return;
    }
    QList<BaseItem*> ItemList(AllItems);
    char buffer[20];
    while(!ItemList.empty()){
        BaseItem* tmp=ItemList.front();
        ItemList.pop_front();
        qDebug()<<tmp->ItemType<<' '<<tmp->penBrush;
        sprintf(buffer, "%d %d\n", tmp->ItemType, tmp->CntPoint);
        file.write(buffer);

        //penRGB, pensize
        sprintf(buffer, "%d %d %d %d\n", tmp->penColor.red(),tmp->penColor.green(),tmp->penColor.blue(), tmp->penSize);
        file.write(buffer);

        //brushRGB, brushStyle
        sprintf(buffer, "%d %d %d %d\n", tmp->penBrush.color().red(),tmp->penBrush.color().green(), tmp->penBrush.color().blue(), tmp->penBrush.style());
        file.write(buffer);

        for(int i=0;i<tmp->CntPoint;i++){
            sprintf(buffer, "%.3f %.3f\n", tmp->ItemPoints[i].x(), tmp->ItemPoints[i].y());
            file.write(buffer);
        }
    }
    file.flush();
    file.close();
}
void DrawScene::OpenCADItems(QString filePath){
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(0 , "FATAL ERROR!" , "FILE OPEN FAIL! PLEASE EXIT! :(", QMessageBox::Ok | QMessageBox::Default , 0, 0 );
        return;
    }
    this->clear();
    int type, CntPoint, penR, penG, penB, pensize, brushR, brushG, brushB, brushStyle;
    double x, y;
    char buffer[30];
    BaseItem *tempItem=nullptr;
    while(!file.atEnd()){
        file.readLine(buffer,30);
        sscanf(buffer, "%d %d", &type, &CntPoint);

        file.readLine(buffer,30);
        sscanf(buffer, "%d %d %d %d",&penR, &penG, &penB, &pensize);

        file.readLine(buffer,30);
        sscanf(buffer, "%d %d %d %d", &brushR, &brushG, &brushB, &brushStyle);

        DefaultPenColor=QColor(penR, penG, penB);
        DefaultPenSize=pensize;
        DefaultBrush.setColor(QColor(brushR,brushG, brushB));
        DefaultBrush.setStyle((Qt::BrushStyle)brushStyle);

        switch((enum::Type)type){
        case line:tempItem = new CADLine(line);break;
        case curve:tempItem=new CADCurve(curve);break;
        case rectangle:tempItem=new CADRect(rectangle);break;
        case circle:tempItem=new CADCircle(circle);break;
        case ellipse:tempItem=new CADEllipse(ellipse);break;
        case polygon:tempItem=new CADPolygon(polygon);break;
        }
        for(int i=0;i<CntPoint;i++){
            file.readLine(buffer,30);
            sscanf(buffer, "%lf %lf", &x, &y);
            tempItem->ItemPoints[i]=QPointF(x, y);
        }
        tempItem->setConstructed(true);
        tempItem->CntPoint=CntPoint;
        tempItem->ControlInit();
        tempItem->form();
        addItem(tempItem->getItem());
    }
    file.close();
}
