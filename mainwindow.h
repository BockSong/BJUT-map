#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsItemAnimation>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QToolButton>
#include <QGraphicsView>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QVector>
#include <QMouseEvent>
#include <QDialog>
#include <QPixmap>
#include <QBitmap>
#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <animation.h>
#include <QTime>
#include <QTimeLine>

static const int MAX = 5000;
static const int MAX_VERTEXNUM = 50;
static const float Pi = 3.14159;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void createToolBar();
    void createAction();
    void setStart(int X, int Y);
    void setEnd(int X, int Y);
    void setNextPos (int index);
    void initScene();

public slots:
    void setStartStation();
    void setEndStation();
    void FindPath(int arg = 0);
    void FindLoop();
    void NewPath();
    void ContinuePath();
    void ChoosePlace();
    void OpenPage();
    void Clear();
    void Clear2();
    void ShowPlaceDialog();
    void ShowInnDialog();

protected:
    void mouseDoubleClickEvent (QMouseEvent *e);//鼠标事件
    void paintEvent(QPaintEvent *event);
    void updateIndex();

private slots:
    void stateChanged(int state);
    void textChanged(const QString &text);

private:
    void SetBtnImage(QPushButton *pBtn,const QString &str);
    void SetAnimation(int nCurveType);
    void getPos(int sX,int sY,int eX,int eY,int count);
    Ui::MainWindow *ui;
    QListWidget *pListWidget;
    QLineEdit *pLineEdit;
    QComboBox *comboBox;
    QString strSelectedText;
    QString strPrint;
    bool bSelected;
    bool Valid;
    QLabel *listLabel;
    QLabel *startLabel;
    QLabel *endLabel;
    QLabel *trafficLabel;
    QLabel *modeLabel;
    QLabel *infoLabel;
    QComboBox *startComboBox;
    QComboBox *endComboBox;
    QComboBox *beginComboBox;
    QComboBox *trafficComboBox;
    QComboBox *modeComboBox;
    QComboBox *InnComboBox;

    QAction *findPathAction;
    QAction *findLoopAction;
    QAction *newPathAction;
    QAction *continueAction;
    QAction *clearAction;
    QAction *callMap;

    QGraphicsScene *scene;
    QGraphicsView *view;

    QPixmap pic1;
    QPixmap pic2;
    QPixmap pic3;
    QPixmap pic4;
    QPixmap pic5;
    QPixmap pic6;
    QPixmap pic7;
    QPixmap pic8;
    QPixmap pic9;
    QPixmap pic10;
    QString strPath;      //文件路径
    QString m_showText;
    QLabel *label;
    QLabel *textlabel;
    QLabel *textlabel2;
    QLabel *textlabel3;
    QTime time;
    AnimationItem *core;
    int m_charWidth;
    int m_curIndex;
    int x_ani;
    int y_ani;
    QGraphicsPixmapItem *mark;

    int startX, startY, endX, endY;
    QVector<int> nextPath;

};

#endif // MAINWINDOW_H
