#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();

    //set to radio choosing
    makeRadio();

    //Create the layout of mainwindow
    createMenus();
    createToolBars();
    createToolSideBars();

    //init my drawing board, with a size of 800*800
    scene = new DrawScene(this);
    scene->setSceneRect(QRectF(0, 0, 800, 800));

    ViewScale=1.2;

    //Init the layout;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    view = new QGraphicsView(scene);
    mainLayout->addWidget(view);
    QWidget *widget = new QWidget;
    widget->setLayout(mainLayout);

    //Antialiasing Rendering
    view->setRenderHint(QPainter::Antialiasing);
    //Cache to accelerate rendering
    view->setCacheMode(QGraphicsView::CacheBackground);

    setCentralWidget(widget);
    setWindowTitle(tr("FCAD"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions(){
    //open
    QIcon iconOpen;
    iconOpen.addFile(QString::fromUtf8(":/images/toload.png"), QSize(), QIcon::Normal, QIcon::Off);
    openFileAct = new QAction(iconOpen,tr("打开"), this);
    openFileAct->setShortcut(tr("Ctrl+O"));
    openFileAct->setStatusTip(tr("打开一个文件"));
    connect(openFileAct,SIGNAL(triggered()),this,SLOT(CADOpenFile()));

    //new
    QIcon iconNew;
    iconNew.addFile(QString::fromUtf8(":/images/derive.png"), QSize(), QIcon::Normal, QIcon::Off);
    newFileAct =new QAction(iconNew,tr("新建"),this);
    newFileAct->setShortcut(tr("Ctrl+N"));
    newFileAct->setStatusTip(tr("新建一个文件"));
    connect(newFileAct,SIGNAL(triggered()),this,SLOT(CADNewFile()));

    //save
    QIcon iconSave;
    iconSave.addFile(QString::fromUtf8(":/images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
    saveFileAct = new QAction(iconSave,tr("保存"),this);
    saveFileAct->setShortcut(tr("Ctrl+S"));
    saveFileAct->setStatusTip(tr("保存文件"));
    connect(saveFileAct, SIGNAL(triggered()), this, SLOT(CADSaveFile()));

    //exit
    exitAct =new QAction(tr("退出"),this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("退出程序"));
    connect(exitAct,SIGNAL(triggered()),this,SLOT(close()));

    //Graph
    QIcon icon0;
    icon0.addFile(QString::fromUtf8(":/images/pointer.png"), QSize(), QIcon::Normal, QIcon::Off);
    JustCursor = new QAction(icon0, tr("光标"), this);
    JustCursor->setCheckable(true);
    connect(JustCursor, SIGNAL(triggered()), this, SLOT(cursorChosen()));

    //Actions of creating a new graph
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/images/line.png"), QSize(), QIcon::Normal, QIcon::Off);
    newLine = new QAction(icon1, tr("线"), this);
    newLine->setCheckable(true);
    connect(newLine, SIGNAL(triggered()), this, SLOT(drawLineChosen()));

    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/images/curve.png"), QSize(), QIcon::Normal, QIcon::Off);
    newCurve = new QAction(icon2, tr("曲线"), this);
    newCurve->setCheckable(true);
    connect(newCurve, SIGNAL(triggered()), this, SLOT(drawCurveChosen()));

    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/images/drawRect.png"), QSize(), QIcon::Normal, QIcon::Off);
    newRegtangle2D = new QAction(icon3, tr("矩形"), this);
    newRegtangle2D->setCheckable(true);
    connect(newRegtangle2D, SIGNAL(triggered()), this, SLOT(drawRectChosen()));

    QIcon iconCircle;
    iconCircle.addFile(QString::fromUtf8(":/images/drawCircle.png"), QSize(), QIcon::Normal, QIcon::Off);
    newCircle2D = new QAction(iconCircle, tr("圆"), this);
    newCircle2D->setCheckable(true);
    connect(newCircle2D, SIGNAL(triggered()), this, SLOT(drawCircleChosen()));

    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/images/ellipse.png"), QSize(), QIcon::Normal, QIcon::Off);
    newEllipse2D = new QAction(icon4, tr("椭圆"), this);
    newEllipse2D->setCheckable(true);
    connect(newEllipse2D, SIGNAL(triggered()), this, SLOT(drawEllipseChosen()));

    QIcon icon5;
    icon5.addFile(QString::fromUtf8(":/images/polygon.png"), QSize(), QIcon::Normal, QIcon::Off);
    newPolygon2D = new QAction(icon5, tr("多边形"), this);
    newPolygon2D->setCheckable(true);
    connect(newPolygon2D, SIGNAL(triggered()), this, SLOT(drawPolygonChosen()));

    //Actions of operation on graph
    QIcon icon6;
    icon6.addFile(QString::fromUtf8(":/images/paintworks.png"), QSize(), QIcon::Normal, QIcon::Off);
    colorSelect = new QAction(icon6, tr("设置颜色"), this);
    connect(colorSelect, SIGNAL(triggered()),this, SLOT(colorSelectChosen()));

    QIcon icon7;
    icon7.addFile(QString::fromUtf8(":/images/fill.png"), QSize(), QIcon::Normal, QIcon::Off);
    fillItem2D = new QAction(icon7, tr("填充"), this);
    connect(fillItem2D, SIGNAL(triggered()), this, SLOT(fillInChosen()));

    QIcon icon8;
    icon8.addFile(QString::fromUtf8(":/images/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
    deleteItem2D = new QAction(icon8, tr("删除"), this);
    connect(deleteItem2D, SIGNAL(triggered()), this, SLOT(DeleteChosen()));

    QIcon icon9;
    icon9.addFile(QString::fromUtf8(":/images/clear.png"), QSize(), QIcon::Normal, QIcon::Off);
    clearAll2D = new QAction(icon9, tr("清除全部"), this);
    connect(clearAll2D, SIGNAL(triggered()), this, SLOT(ClearChosen()));

    //view
    QIcon icon10;
    icon10.addFile(QString::fromUtf8(":/images/zoomin.png"), QSize(), QIcon::Normal, QIcon::Off);
    zoomInAct = new QAction(icon10,tr("放大"), this);
    zoomInAct->setShortcut(tr("Ctrl+I"));
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(ZoomInChosen()));

    QIcon icon11;
    icon11.addFile(QString::fromUtf8(":/images/zoomout.png"), QSize(), QIcon::Normal, QIcon::Off);
    zoomOutAct = new QAction(icon11,tr("缩小"), this);
    zoomOutAct->setShortcut(tr("Ctrl+O"));
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(ZoomOutChosen()));

    QIcon icon12;
    icon12.addFile(QString::fromUtf8(":/images/bringtofront.png"), QSize(), QIcon::Normal, QIcon::Off);
    bringUp = new QAction(icon12,tr("图层上移"), this);
    connect(bringUp, SIGNAL(triggered()), this, SLOT(BringUpChosen()));

    QIcon icon13;
    icon13.addFile(QString::fromUtf8(":/images/sendtoback.png"), QSize(), QIcon::Normal, QIcon::Off);
    bringDown= new QAction(icon13,tr("图层下移"), this);
    connect(bringDown, SIGNAL(triggered()), this, SLOT(BringDownChosen()));

    QIcon icon14;
    icon14.addFile(QString::fromUtf8(":/images/undo.png"), QSize(), QIcon::Normal, QIcon::Off);
    undo = new QAction(icon14, tr("撤销"), this);
    connect(undo, SIGNAL(triggered()), this, SLOT(undoChosen()));

    QIcon icon15;
    icon15.addFile(QString::fromUtf8(":/images/redo.png"), QSize(), QIcon::Normal, QIcon::Off);
    redo = new QAction(icon15, tr("重做"), this);
    connect(redo, SIGNAL(triggered()), this, SLOT(redoChosen()));


    helpUse = new QAction(tr("如何使用?"), this);
    connect(helpUse, SIGNAL(triggered()), this, SLOT(helpChosen()));

    //线宽控制
    LineWidthLabel = new QLabel(tr("线宽"));
    LineWidth = new QSpinBox;
    LineWidth->setRange(0,20);
    LineWidth->setValue(DefaultPenSize);
    connect(LineWidth, SIGNAL(valueChanged(int)), this, SLOT(LineWidthChanged(int)));

    //画刷风格
    BrushStyleLabel = new QLabel(tr("画刷风格"));
    BrushStyle = new QComboBox;
    BrushStyle->addItem(tr("NoBrush"), static_cast<int>(Qt::NoBrush));
    BrushStyle->addItem(tr("SolidPattern"),static_cast<int>(Qt::SolidPattern));
    BrushStyle->addItem(tr("Dense1Pattern"),static_cast<int>(Qt::Dense1Pattern));
    BrushStyle->addItem(tr("Dense2Pattern"),static_cast<int>(Qt::Dense2Pattern));
    BrushStyle->addItem(tr("Dense3Pattern"),static_cast<int>(Qt::Dense3Pattern));
    BrushStyle->addItem(tr("Dense4Pattern"),static_cast<int>(Qt::Dense4Pattern));
    BrushStyle->addItem(tr("Dense5Pattern"),static_cast<int>(Qt::Dense5Pattern));
    BrushStyle->addItem(tr("Dense6Pattern"),static_cast<int>(Qt::Dense6Pattern));
    BrushStyle->addItem(tr("Dense7Pattern"),static_cast<int>(Qt::Dense7Pattern));
    BrushStyle->addItem(tr("HorPattern"),static_cast<int>(Qt::HorPattern));
    BrushStyle->addItem(tr("VerPattern"),static_cast<int>(Qt::VerPattern));
    BrushStyle->addItem(tr("CrossPattern"),static_cast<int>(Qt::CrossPattern));
    BrushStyle->addItem(tr("BDiagPattern"),static_cast<int>(Qt::BDiagPattern));
    BrushStyle->addItem(tr("FDiagPattern"),static_cast<int>(Qt::FDiagPattern));
    BrushStyle->addItem(tr("DiagCrossPattern"),static_cast<int>(Qt:: DiagCrossPattern));
    BrushStyle->addItem(tr("LinearGradientPattern"),static_cast<int>(Qt:: LinearGradientPattern));
    BrushStyle->addItem(tr("ConicalGradientPattern"),static_cast<int>(Qt:: ConicalGradientPattern));
    BrushStyle->addItem(tr("RadialGradientPattern"),static_cast<int>(Qt:: RadialGradientPattern));
    BrushStyle->addItem(tr("TexturePattern"),static_cast<int>(Qt::TexturePattern));
    connect(BrushStyle,SIGNAL(activated(int)),this,SLOT (BrushStyleChanged(int)));
}

void MainWindow::makeRadio(){
    drawActionRadio = new QActionGroup(this);
    drawActionRadio->addAction(JustCursor);
    drawActionRadio->addAction(newLine);
    drawActionRadio->addAction(newCurve);
    drawActionRadio->addAction(newRegtangle2D);
    drawActionRadio->addAction(newCircle2D);
    drawActionRadio->addAction(newEllipse2D);
    drawActionRadio->addAction(newPolygon2D);
}

void MainWindow::createMenus(){
    //file Menu
    fileMenu = menuBar()->addMenu(tr("文件"));
    fileMenu->addAction(newFileAct);
    fileMenu->addAction(openFileAct);
    fileMenu->addAction(saveFileAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    //edit Menu
    editMenu = menuBar()->addMenu(tr("编辑"));
    editMenu->addAction(newLine);
    editMenu->addAction(newCurve);
    editMenu->addAction(newRegtangle2D);
    editMenu->addAction(newEllipse2D);
    editMenu->addAction(newPolygon2D);

    //view Menu
    viewMenu = menuBar()->addMenu(tr("视图"));
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);

    //About Menu
    aboutMenu = menuBar()->addMenu(tr("关于"));
    aboutMenu->addAction(helpUse);

    //status Bar
    tempStatus = new QLabel("WelCome!");
    tempStatus->setAlignment(Qt::AlignHCenter);
    permStatus = new QLabel("初始化完成！");
    statusBar()->addPermanentWidget(permStatus);
    statusBar()->addWidget(tempStatus);
}


void MainWindow::createToolBars(){
    toolBar = new QToolBar(this);
    this->addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar->addAction(JustCursor);
    toolBar->addAction(newLine);
    toolBar->addAction(newCurve);
    toolBar->addAction(newRegtangle2D);
    toolBar->addAction(newCircle2D);
    toolBar->addAction(newEllipse2D);
    toolBar->addAction(newPolygon2D);
    toolBar->addSeparator();

    toolBar->addAction(zoomInAct);
    toolBar->addAction(zoomOutAct);
    toolBar->addSeparator();

    toolBar->addAction(colorSelect);
    toolBar->addAction(fillItem2D);
    toolBar->addAction(bringUp);
    toolBar->addAction(bringDown);
    toolBar->addSeparator();

    toolBar->addAction(deleteItem2D);
    toolBar->addAction(clearAll2D);
    toolBar->addSeparator();

    toolBar->addAction(undo);
    toolBar->addAction(redo);
}

void MainWindow::createToolSideBars(){
    QHBoxLayout* toolDock = new QHBoxLayout;
    toolDock->addWidget(LineWidthLabel);
    toolDock->addWidget(LineWidth);
    toolDock->addWidget(BrushStyleLabel);
    toolDock->addWidget(BrushStyle);

    QWidget *sideBars = new QWidget;
    sideBars->setLayout(toolDock);

    toolSideBar = new QToolBar(this);
    this->addToolBar(Qt::TopToolBarArea, toolSideBar);
    toolSideBar->addWidget(sideBars);
}

void MainWindow::cursorChosen(){
    scene->setMode(justcursor);
    tempStatus->setText("选择更改图形状态");
    permStatus->setText("当前光标：功能光标");
}

void MainWindow::drawLineChosen(){
    scene->setMode(createnew);
    scene->setItemType(line);
    tempStatus->setText("左键长按绘制");
    permStatus->setText("当前光标：新建线");
}
void MainWindow::drawCurveChosen(){
    scene->setMode(createnew);
    scene->setItemType(curve);
    tempStatus->setText("左键长按绘制");
    permStatus->setText("当前光标：新建曲线");
}
void MainWindow::drawRectChosen(){
    scene->setMode(createnew);
    scene->setItemType(rectangle);
    tempStatus->setText("左键长按绘制");
    permStatus->setText("当前光标：新建矩形");
}
void MainWindow::drawCircleChosen(){
    scene->setMode(createnew);
    scene->setItemType(circle);
    tempStatus->setText("左键长按绘制");
    permStatus->setText("当前光标：新建圆");
}
void MainWindow::drawEllipseChosen(){
    scene->setMode(createnew);
    scene->setItemType(ellipse);
    tempStatus->setText("左键长按绘制");
    permStatus->setText("当前光标：新建椭圆");
}
void MainWindow::drawPolygonChosen(){
    scene->setMode(createnew);
    scene->setItemType(polygon);
    tempStatus->setText("左键单击开始绘制，右键单击结束绘制，请勿中途切换图形");
    permStatus->setText("当前光标：新建多边形");
}

void MainWindow::DeleteChosen(){
    if(selectedItem){
        scene->removeItem(selectedItem->getItem());
        AllItems.removeOne(selectedItem);
        Trash.append(selectedItem);
        selectedItem=nullptr;
        tempStatus->setText("删除成功！");
    }
}

void MainWindow::ClearChosen(){
    scene->clear();
    selectedItem=nullptr;
    AllItems.clear();
    tempStatus->setText("清空完成！");
}

void MainWindow::ZoomInChosen(){
    view->scale(ViewScale, ViewScale);
    tempStatus->setText("画布放大！");
}
void MainWindow::ZoomOutChosen(){
    view->scale(1/ViewScale, 1/ViewScale);
    tempStatus->setText("画布缩小！");
}

void MainWindow::colorSelectChosen(){
    DefaultPenColor=QColorDialog::getColor(Qt::white, this);
    if(selectedItem){
        selectedItem->setMyPen(DefaultPenColor, DefaultPenSize);
        selectedItem->penColor=DefaultPenColor;
    }
    tempStatus->setText("默认画笔颜色已修改！");
}
void MainWindow::fillInChosen(){
    DefaultBrush.setColor(QColorDialog::getColor(Qt::white, this));
    if(selectedItem){
        selectedItem->setMyBrush(DefaultBrush);
        selectedItem->penBrush=DefaultBrush;
    }
    tempStatus->setText("默认填充已经修改！如果没有变化，记得调整画刷风格！");
}

void MainWindow::BringUpChosen(){
    if(selectedItem){
        selectedItem->setZvalue(selectedItem->getZvalue()+1);
        tempStatus->setText("图层上移！如果图层太低可能需要多按几次才有效果");
    }
    else tempStatus->setText("没有选取图形！");
}
void MainWindow::BringDownChosen(){
    if(selectedItem&&selectedItem->getZvalue()>0){
        selectedItem->setZvalue(selectedItem->getZvalue()-1);
        tempStatus->setText("图层下移！如果图层太高可能需要多按几次才有效果");
    }
    else tempStatus->setText("没有选取图形！");
}

void MainWindow::LineWidthChanged(int size){
    DefaultPenSize=size;
    if(selectedItem){
        selectedItem->setMyPen(DefaultPenColor, DefaultPenSize);
        selectedItem->penSize=DefaultPenSize;
    }
    tempStatus->setText("画笔粗细已修改！");
}

void MainWindow::BrushStyleChanged(int style){
    DefaultBrush.setStyle((Qt::BrushStyle)style);
    if(selectedItem){
        selectedItem->setMyBrush(DefaultBrush);
        selectedItem->penBrush=DefaultBrush;
    }
    tempStatus->setText("画刷风格已修改！");
}

void MainWindow::CADOpenFile(){
    QString filePath = QFileDialog::getOpenFileName(this,tr("打开"),tr(""),tr("CAD文件(*.CAD)"));
    if(filePath == "") return;
    scene->OpenCADItems(filePath);
    tempStatus->setText("打开文件！");
    permStatus->setText("文件路径："+filePath);
}
void MainWindow::CADNewFile(){
    MainWindow *w = new MainWindow;
    w->show();
    tempStatus->setText("新建文件！");
    permStatus->setText("有新窗口啦！");
}
void MainWindow::CADSaveFile(){
    QString filePath = QFileDialog::getSaveFileName(this,tr("另存"),tr("test.CAD"),tr("CAD文件(*.CAD)"));
    if(filePath == "") return;
    scene->SaveCADItems(filePath);
    tempStatus->setText("保存文件");
    permStatus->setText("文件路径："+filePath);
}

void MainWindow::undoChosen(){
    if(!AllItems.empty()){
        Trash.append(AllItems.back());
        scene->removeItem(AllItems.back()->getItem());
        AllItems.pop_back();
        tempStatus->setText("已撤销！");
    }
    else tempStatus->setText("没有图形撤销啦！");
}

void MainWindow::redoChosen(){
    if(!Trash.empty()){
        AllItems.append(Trash.back());
        scene->addItem(Trash.back()->getItem());
        Trash.pop_back();
        tempStatus->setText("已恢复！");
    }
    else tempStatus->setText("没有图形恢复啦！");
}

void MainWindow::helpChosen(){
    QString title("如何使用？");
    QString content = "工具栏介绍：\n"
                      "1.光标 即功能光标，单击可以选中图形，此时图形边框变绿变粗，通过拖动控制点进行控制；也可以改变颜色和画笔粗细等\n"
                      "2.各种图形：选中后，按住鼠标拖动即可生成；注意：多边形通过左键点击生成，右键点击结束绘制，与其他图形有区别\n"
                      "3.放大镜控制画布放缩；画笔和笔刷控制线框和填充颜色；图层控制图形的图层顺序\n"
                      "4.红叉删除选中图形，垃圾桶删除所有图形\n"
                      "5.撤销可以撤销刚刚的绘图，重做可以重现刚刚撤销的绘图\n"
                      "6.选中图形后，灰色点控制边移动，红色点控制平移，蓝色点控制旋转和放缩\ns"
                      "操作过程中，状态栏也会有相应的提示信息！";
     QMessageBox::critical(0 , title , content, QMessageBox::Ok | QMessageBox::Default , 0, 0 );
}
