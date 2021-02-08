#include "baseitem.h"

BaseItem* selectedItem = nullptr;
QColor DefaultPenColor=Qt::black;
int DefaultPenSize=2;
QBrush DefaultBrush(Qt::white, Qt::NoBrush);

QList<BaseItem*> AllItems;
QList<BaseItem*> Trash;
