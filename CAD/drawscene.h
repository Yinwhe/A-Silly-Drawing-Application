#ifndef DRAWSCENE_H
#define DRAWSCENE_H
#include <QGraphicsScene>
#include <QGraphicsItem>
#include "baseitem.h"
#include "cadline.h"
#include "cadrect.h"
#include "cadcircle.h"
#include "cadellipse.h"
#include "cadpolygon.h"
#include "cadcurve.h"
using namespace std;

class DrawScene:public QGraphicsScene
{
    Q_OBJECT
public:
    DrawScene(QObject *parent = nullptr);

    void setMode(Mode mode);
    void setItemType(Type type);

    void SaveCADItems(QString filePath);
    void OpenCADItems(QString filePath);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    Type currentType;
    Mode currentMode;
    BaseItem *tempItem;
};

#endif // DRAWSCENE_H
