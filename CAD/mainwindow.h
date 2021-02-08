#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "drawscene.h"
#include <QMainWindow>
#include <QGraphicsView>
#include <QActionGroup>
#include <QColorDialog>
#include <QMessageBox>
#include <QSpinBox>
#include <QComboBox>
#include <QToolBar>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QDockWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QFile>
#include <QFileDialog>
#include <QStatusBar>
using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createToolSideBars();
    void makeRadio();

private:
    DrawScene *scene;
    QGraphicsView *view;
    //Menu Part
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *aboutMenu;

    //Tool
    QToolBar *toolBar;
    QToolBar *toolSideBar;

    //Statues
    QLabel *tempStatus;
    QLabel *permStatus;

    //File Action
    QAction *newFileAct;
    QAction *openFileAct;
    QAction *saveFileAct;
    QAction *exitAct;

    //Edit button
    QAction *JustCursor;
    QAction *newLine;
    QAction *newCurve;
    QAction *newRegtangle2D;
    QAction *newCircle2D;
    QAction *newEllipse2D;
    QAction *newPolygon2D;

    QAction *colorSelect;
    QAction *fillItem2D;
    QAction *deleteItem2D;
    QAction *clearAll2D;

    QAction *bringUp;
    QAction *bringDown;

    //View Action
    QAction *zoomInAct;
    QAction *zoomOutAct;

    QAction *undo;
    QAction *redo;

    //Help Action
    QAction *helpUse;


    QLabel *LineWidthLabel;
    QSpinBox *LineWidth;

    QLabel *BrushStyleLabel;
    QComboBox *BrushStyle;

    QActionGroup *drawActionRadio;

    qreal ViewScale;
private slots:
    void cursorChosen();
    void drawLineChosen();
    void drawCurveChosen();
    void drawRectChosen();
    void drawCircleChosen();
    void drawEllipseChosen();
    void drawPolygonChosen();
    void DeleteChosen();
    void ClearChosen();
    void colorSelectChosen();
    void fillInChosen();
    void ZoomInChosen();
    void ZoomOutChosen();
    void BringUpChosen();
    void BringDownChosen();
    void LineWidthChanged(int);
    void BrushStyleChanged(int);
    void undoChosen();
    void redoChosen();
    void helpChosen();


    void CADOpenFile();
    void CADNewFile();
    void CADSaveFile();
};
#endif // MAINWINDOW_H
