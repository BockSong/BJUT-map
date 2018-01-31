#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "location.h"
#include <navigation.h>
#include <qdebug.h>
#include <QToolBar>
#include <QtAlgorithms>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <iomanip>
#include <math.h>
#include <conio.h>
#include <QListWidgetItem>
#include <QTextCodec>
#include <QCheckBox>
#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <windows.h>
#include <QTimer>
#include <QDesktopServices>
using namespace std;

QTextCodec *codec = QTextCodec::codecForName("GBK");

const int w_widget = 928;
const int h_widget = 928;
const int space_widget = 550;

int starth, startmin, staymin;
int temph, tempmin;
int counti;                                                       //分步显示计数器
int spindex;                                                      //上课地点序号
int record[MAX_VERTEXNUM];
int i, j, index, traffic, stayminlocal, mode;
int classstarth, classstartmin, classendh, classendmin;
int earlyh, earlymin, lateh, latemin;
int tempn, tempg[MAX_VERTEXNUM][MAX_VERTEXNUM];                   //存储读取到的地图信息作为备用
double costmin;
//回溯法（回路搜索）
int vex[MAX_VERTEXNUM], bestvex[MAX_VERTEXNUM];                   //vex,bestvex存放所有地点和最佳路线的序号
int curlen = 0, bestlen = MAX, n;                                 //curlen, bestlen记录已走长度，最短回路长度
//dijkstra算法（单源搜索）
int prevex[MAX_VERTEXNUM];                                        //最短路上的前驱顶点
int d[MAX_VERTEXNUM];                                             //边e = (u,v)的权值(不存在时为MAX. d[i][i]=0)
bool used[MAX_VERTEXNUM];                                         //用过的点的集合

char ch;
char adname[10];
char tempstring[10];

FILE *fp;
QString astring;
MyGraph mgraph;                                                   //图结构的声明
place sites[MAX_VERTEXNUM];
MapAlgorithm *mapbjut;

char* ReadName(FILE* fp) {
    char ch;
    while (true)
    {
        ch = fgetc(fp);
        if (ch == '#') {
            fscanf(fp, "%s", adname);
            return adname;
        }
    }
}

int addRecord(int value, int n) {
    int i = 0;
    for (; i<n; i++) {
        if (record[i] == 0)
            break;
    }
    if (i == n)
        return 0;
    else {
        record[i] = value;
        return 1;
    }
}

int calh(int starth, int startmin, int costmin, int type=0) {
    if(type==0)
        return starth + (startmin + costmin) / 60;
    else if(startmin-costmin < 0)
        return calh(starth-1, startmin+60, costmin, 1);
    return starth;
}

int calmin(int startmin, int costmin, int type=0) {
    if(type==0)
        return (startmin + costmin) % 60;
    else if(startmin-costmin < 0)
        return startmin+60-costmin;
    return startmin-costmin;
}

int caldiff(int h1,int m1,int h2,int m2){
    return (h2-h1)*60+m2-m1;
}

void calcost(int type, int distance) {
    switch (type) {
    case 1:
        costmin = distance * 100 / 72.0;//步行速度为72m/s
        break;
    case 2:
        costmin = distance * 100 / 300.0;//骑行速度为300m/s
        break;
    case 3:
        costmin = distance * 100 / 500.0 + n*0.25 + 0.5;//校内行车速度为500m/s，减速耗时15s/每地点，找停车位耗时30s
        break;
    default:
        break;
    }
}

void calstay() {
    staymin = 0;
    for (int i = 1; i <= n; i++) {
        staymin += sites[record[bestvex[i] - 1]].time;
    }
}

void init(int modelocal=0) {
    curlen = 0;
    bestlen = MAX;
    starth = temph;
    startmin = tempmin;
    memset(mgraph.arcs, MAX, sizeof(mgraph.arcs));
    memset(record, 0, sizeof(record));
    for (i = 1; i <= tempn; i++)
    {
        ::vex[i] = i;
        bestvex[i] = 0;
    }
    if(modelocal==1)
        return;
    n = tempn;
    for (int i = 1; i <= mgraph.vexnum; i++) {
        for (int j = 1; j <= mgraph.vexnum; j++) {
            if (tempg[i][j] != -1)
                mgraph.arcs[i][j] = tempg[i][j];
            else
                mgraph.arcs[i][j] = MAX;
        }
    }
    for (i = 0; i < n; i++)
        record[i] = i + 1;
}

void Backtracking(int t,int vil,double t1,double t2,int type)
{
    int j;
    if(t>n) //到达最后一个地点
    {
        if(mgraph.arcs[vex[n]][1]!=-1 && (curlen+mgraph.arcs[vex[n]][1]<bestlen))//检验是否是最优解
        {
            for(j=1; j<=n; j++)
                bestvex[j]=vex[j];
            bestlen=curlen+mgraph.arcs[vex[n]][1];
        }
    }
    else
    {
        for(j=t; j<=n; j++)//按照DFS的策略搜索当前节点的所有可达节点
        {
            if(mgraph.arcs[vex[t-1]][vex[j]]!=-1 && (curlen+mgraph.arcs[vex[t-1]][vex[j]]<bestlen))//检验是否有必要搜索该节点
            {
                calcost(type,mgraph.arcs[vex[t-1]][vex[j]]);
                if(vex[j]==vil&&((t1-costmin)>0||(t2-costmin)<0))//时间限制的约束条件
                    continue;
                swap(vex[t],vex[j]); //试探节点vex[j]并将其加入已探索点集,将节点vex[t]移入待探索点集
                curlen+=mgraph.arcs[vex[t-1]][vex[t]];
                Backtracking(t+1,vil,t1-costmin-sites[vex[t]].time,t2-costmin-sites[vex[t]].time,type);//搜索下个地点
                curlen-=mgraph.arcs[vex[t-1]][vex[t]];  //回溯
                swap(vex[t],vex[j]);
            }
        }
    }
}


void dijkstra (int startPos)
{
    for (int i = 1; i <= mgraph.vexnum; i++) d[i] = MAX;
    for (int i = 1; i <= mgraph.vexnum; i++) used[i] = false;
    for (int i = 1; i <= mgraph.vexnum; i++) prevex[i] = -1;
    d[startPos] = 0;

    while (true) {
        int v = -1;
        for (int u = 1; u <= mgraph.vexnum; u++) {
            if (!used[u] && (v == -1 || d[u] < d[v])) v = u;
        }

        if (v == -1) break;
        used[v] = true;

        for (int u = 1; u <= mgraph.vexnum; u++) {
            if (d[u] > d[v] + mgraph.arcs[v][u]) {
                d[u] = d[v] + mgraph.arcs[v][u];
                prevex[u] = v;
            }
        }
    }
}

MapAlgorithm::MapAlgorithm()
{
       mgraph.vexnum = tempn;                        //初始化顶点数目
}

void MapAlgorithm::CreateGraph ()
{
    for (int i = 1; i <= mgraph.vexnum; i++) {
        for (int j = 1; j <= mgraph.vexnum; j++) {
            if (tempg[i][j] != -1)
                mgraph.arcs[i][j] = tempg[i][j];
            else
                mgraph.arcs[i][j] = MAX;
        }
    }
}

QVector<int> MapAlgorithm::getpath_single (int endPos)
{
    QVector<int> path;

    for ( ; endPos != -1; endPos = prevex[endPos]) {
        path.push_back (endPos);
    }
    std::reverse(path.begin (), path.end ());
    return path;
}

QVector<int> MapAlgorithm::getpath_loop ()
{
    QVector<int> path;

    for (int index=2 ; index<=n; index++) {
        path.push_back (record[bestvex[index]-1]-1);
    }
    path.push_back(record[0]-1);
    return path;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_curIndex = 0;//当前角码
    m_showText = "欢迎使用BJUTmap!界面左侧为工具栏，右侧为地图显示；Double Click地点名称可以查看详细内容. 祝您使用愉快!";//显示的文字
    m_charWidth = fontMetrics().width("a");//每个字符的宽度

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateIndex);
    timer->start(100);
    Valid=false;

    fp = fopen("DSAdata.txt", "r");    //打开文字文件只读
    if (!fp) {
        qDebug() << "Open file error";
        exit(1);
    }
    tempn = Read(fp);
    spindex = Read(fp);
    qDebug() << "地图邻接矩阵";
    for (i = 1; i <= tempn; i++) {
        for (j = 1; j <= tempn; j++) {
            tempg[i][j] = Read(fp);
            cout << tempg[i][j] << " ";
        }
        cout << endl;
    }
    for (i = 1; i <= tempn; i++) {
        strcpy(sites[i].name, ReadName(fp));
        ch = fgetc(fp);
        sites[i].time = Read(fp);
    }
    temph = Read(fp);
    tempmin = Read(fp);
    classstarth = Read(fp);
    classstartmin = Read(fp);
    classendh = Read(fp);
    classendmin = Read(fp);

    int tempdiff=caldiff(classstarth, classstartmin, classendh, classendmin);
    earlyh = calh(classstarth, classstartmin, sites[spindex].time-tempdiff, 1);
    earlymin = calmin(classstartmin, sites[spindex].time-tempdiff, 1);
    lateh = classstarth;
    latemin = classstartmin;
    starth = temph;
    startmin = tempmin;

    strPrint.sprintf("当前时刻: %d:%d", temph, tempmin);
    ui->textBrowser->append(strPrint);
    ui->textBrowser->setTextColor(QColor(0,0,0));

    for (i = 1; i <= tempn; i++)
    {
        ::vex[i] = i;
        bestvex[i] = 0;
    }
    n = tempn;
    for (i = 1; i <= n; i++)
        for (j = 1; j <= n; j++)
            mgraph.arcs[i][j] = tempg[i][j];
    for (i = 0; i < n; i++)
        record[i] = i + 1;


    setWindowTitle ("BJUT MAP");
    mapbjut = new MapAlgorithm();
    mapbjut->CreateGraph ();

    scene = new QGraphicsScene;
    scene->setSceneRect (-100, -100, 950, 725);
    initScene();

    view = new QGraphicsView;
    view->setScene (scene);
    view->setMinimumSize (1800, 1000);
    view->show ();
    setCentralWidget (view);

    createAction ();
    createToolBar ();           //工具栏
    setMinimumSize (1800, 1000);  //设置最小尺寸

    Sleep(1000);
}

//设置按钮图像以及透明
void MainWindow::SetBtnImage(QPushButton *pBtn,const QString &str)
{
    //pBtn->setStyleSheet("QToolButton{border:0px;}");
    pBtn->resize(w_widget,h_widget);
    pBtn->setIconSize(QSize(w_widget,h_widget));
    QPixmap objPixmap(str);
    pBtn->setIcon(QIcon(objPixmap));
    pBtn->setMask(objPixmap.mask());
}


void MainWindow::initScene ()
{
    QGraphicsPixmapItem *item =
            scene->addPixmap (QPixmap("BJUTfinal.jpg"));
    item->setPos (-500, -250);
    QGraphicsPixmapItem *star =
            scene->addPixmap (QPixmap("star.png"));
    star->setPos (435, -60);

    mark = scene->addPixmap (QPixmap("images/animation.png"));
    mark->setFlag(QGraphicsItem::ItemIsMovable);
    mark->setPos (-480, -200);
    AnimationItem *newstar = new AnimationItem;
    QGraphicsItemAnimation *anim = new QGraphicsItemAnimation;
    anim->setItem(newstar);
    QTimeLine *timeLine = new QTimeLine(4000);
    timeLine->setCurveShape(QTimeLine::SineCurve);
    timeLine->setLoopCount(0);
    anim->setTimeLine(timeLine);
    int y = 485;
    for (int i=0; i<400; i++)
    {
        anim->setPosAt(i/330.0, QPointF(i-430,y));
    }
    timeLine->start();
    scene->addItem(newstar);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createAction ()
{
    findPathAction = new QAction(QIcon("Search1to1.png"),tr("搜索路线"), this);
    findPathAction->setShortcut (tr("Ctrl+F"));
    findPathAction->setStatusTip (tr("搜索路径"));
    connect (findPathAction, SIGNAL(triggered(bool)), this, SLOT(FindPath()));

    findLoopAction = new QAction(QIcon("map.png"),tr("总路线图"), this);
    findLoopAction->setShortcut (tr("Ctrl+T"));
    findLoopAction->setStatusTip (tr("开始寻路."));
    connect (findLoopAction, SIGNAL(triggered(bool)), this, SLOT(FindLoop()));

    newPathAction = new QAction(QIcon("Search.png"),tr("新的导航"), this);
    newPathAction->setShortcut (tr("Ctrl+W"));
    newPathAction->setStatusTip (tr("下一步."));
    connect (newPathAction, SIGNAL(triggered(bool)), this, SLOT(NewPath()));

    continueAction = new QAction(QIcon("Next.png"),tr("继续导航"), this);
    continueAction->setShortcut (tr("Ctrl+W"));
    continueAction->setStatusTip (tr("下一步."));
    connect (continueAction, SIGNAL(triggered(bool)), this, SLOT(ContinuePath()));

    clearAction = new QAction(QIcon("Clear.png"), tr("刷新地图"), this);
    clearAction->setShortcut (tr("Ctrl+W"));
    clearAction->setStatusTip (tr("清理路径"));
    connect (clearAction, SIGNAL(triggered(bool)), this, SLOT(Clear2()));
}

void MainWindow::createToolBar ()
{
    QFont labelfont("微软雅黑", 12);
    labelfont.setBold (true);
    //QGridLayout *barlayout = new QGridLayout(ui->mainToolBar);
    QLabel *empty = new QLabel(tr(" \n "));
    startLabel = new QLabel(tr("起点"));
    startComboBox = new QComboBox;
    for(int counttool=1;counttool<=tempn;counttool++){
        astring=codec->toUnicode(sites[counttool].name);
        startComboBox->addItem (astring);  //1
    }
    trafficLabel = new QLabel(tr("            出行方式"));
    trafficComboBox = new QComboBox;
    trafficComboBox->addItem (tr("步行"));  //0
    trafficComboBox->addItem (tr("骑车"));  //1
    trafficComboBox->addItem (tr("汽车"));  //2

    modeLabel = new QLabel(tr("            导航模式"));
    modeComboBox = new QComboBox;
    modeComboBox->addItem (tr("游客模式"));  //0
    modeComboBox->addItem (tr("学生模式"));  //1
    modeComboBox->addItem (tr("自定模式"));  //2
    connect(modeComboBox, SIGNAL(activated(int)), this, SLOT(ShowPlaceDialog()));

    endLabel = new QLabel(tr("                                终点"));
    endComboBox = new QComboBox;
    for(int counttool=1;counttool<=tempn;counttool++){
        astring=codec->toUnicode(sites[counttool].name);
        endComboBox->addItem (astring);  //1
    }
    setStart(-267, -468);
    setEnd(-267, -468);
    connect (startComboBox, SIGNAL(activated(int)), this, SLOT(setStartStation()));
    connect (endComboBox, SIGNAL(activated(int)), this, SLOT(setEndStation()));

    infoLabel = new QLabel(tr("      导航方案"));

    startLabel->setFont(labelfont);
    endLabel->setFont(labelfont);
    modeLabel->setFont(labelfont);
    trafficLabel->setFont(labelfont);
    startLabel->setStyleSheet("color:#7A378B;");
    endLabel->setStyleSheet("color:#7A378B;");
    modeLabel->setStyleSheet("color:#7A378B;");
    trafficLabel->setStyleSheet("color:#7A378B;");
    ui->toolBar->addWidget (empty);
    ui->mainToolBar->addWidget (modeLabel);
    ui->mainToolBar->addWidget (modeComboBox);
    ui->mainToolBar->addSeparator ();
    ui->mainToolBar->addWidget (trafficLabel);
    ui->mainToolBar->addWidget (trafficComboBox);
    ui->mainToolBar->addSeparator ();
    ui->mainToolBar->addWidget (startLabel);
    ui->mainToolBar->addWidget (startComboBox);
    ui->mainToolBar->addSeparator ();
    ui->mainToolBar->addWidget (endLabel);
    ui->mainToolBar->addWidget (endComboBox);
    ui->mainToolBar->addSeparator ();
    ui->mainToolBar->addAction(findPathAction);
    ui->mainToolBar->addSeparator ();
    ui->mainToolBar->addAction(findLoopAction);
    ui->mainToolBar->addSeparator ();
    ui->mainToolBar->addAction(newPathAction);
    ui->mainToolBar->addSeparator ();
    ui->mainToolBar->addAction(continueAction);
    ui->mainToolBar->addSeparator ();
    ui->mainToolBar->addAction(clearAction);
    ui->mainToolBar->addSeparator ();
    ui->mainToolBar->addWidget (infoLabel);
    ui->mainToolBar->addSeparator ();
    ui->mainToolBar->addWidget (ui->textBrowser);
}

void MainWindow::setStart(int X, int Y) {
    startX = X; startY = Y;
}

void MainWindow::setEnd (int X, int Y)
{
    endX = X; endY = Y;
}

void MainWindow::setStartStation ()
{
    switch (startComboBox->currentIndex ()) {
    case 0:
        setStart (-267, -468); break;
    case 1:
        setStart (-430, -405); break;
    case 2:
        setStart (-379, -214); break;
    case 3:
        setStart (-589, -132); break;
    case 4:
        setStart (-450, -55); break;
    case 5:
        setStart (-188, 54); break;
    case 6:
        setStart (21, 55); break;
    case 7:
        setStart (-312, -54); break;
    case 8:
        setStart (-505, -468); break;
    case 9:
        setStart (-320, -575); break;
    default:
        break;
    }
}

void MainWindow::setEndStation ()
{
    switch (endComboBox->currentIndex ()) {
    case 0:
        setEnd(-267, -468); break;
    case 1:
        setEnd (-430, -405); break;
    case 2:
        setEnd (-379, -214); break;
    case 3:
        setEnd (-589, -132); break;
    case 4:
        setEnd (-450, -55); break;
    case 5:
        setEnd (-188, 54); break;
    case 6:
        setEnd (21, 55); break;
    case 7:
        setEnd (-312, -54); break;
    case 8:
        setEnd (-505, -468); break;
    case 9:
        setEnd (-320, -575); break;
    default:
        break;
    }
}

void MainWindow::setNextPos (int index)
{
    switch (index) {
    case 0:
        setEnd(-267, -468); break;
    case 1:
        setEnd (-430, -405); break;
    case 2:
        setEnd (-379, -214); break;
    case 3:
        setEnd (-589, -132); break;
    case 4:
        setEnd (-450, -55); break;
    case 5:
        setEnd (-188, 54); break;
    case 6:
        setEnd (21, 55); break;
    case 7:
        setEnd (-312, -54); break;
    case 8:
        setEnd (-505, -468); break;
    case 9:
        setEnd (-320, -575); break;
    default:
        break;
    }
}

void MainWindow::FindPath (int arg)
{
    for (int i = 1; i <= mgraph.vexnum; i++) {
        for (int j = 1; j <= mgraph.vexnum; j++) {
            if (tempg[i][j] != -1)
                mgraph.arcs[i][j] = tempg[i][j];
            else
                mgraph.arcs[i][j] = MAX;
        }
    }
    counti=0;

    dijkstra (startComboBox->currentIndex ()+1);//设置下一处的终点
    if(arg == 0)
        nextPath = mapbjut->getpath_single (endComboBox->currentIndex ()+1);
    else
        nextPath = mapbjut->getpath_single ( arg );

    Clear ();
    //绘制路线
    QGraphicsPathItem *item = new QGraphicsPathItem();
    QGraphicsPathItem *item2 = new QGraphicsPathItem();

    QPen pen,pen2;
    pen.setWidth (5);
    pen.setColor (Qt::red);
    item->setPen (pen);
    item->setFlag (QGraphicsItem::ItemIsPanel);
    pen2.setWidth (8);
    pen2.setColor (Qt::blue);
    item2->setPen (pen2);
    item2->setBrush(Qt::blue);
    item2->setFlag (QGraphicsItem::ItemIsPanel);


    for (int i = 0; i < nextPath.size (); i++) {
        qDebug() << nextPath[i] << " , ";
    }
    scene->addItem (item);
    scene->addItem (item2);

    QPainterPath pa,pa2;               //path

    pa.moveTo (startX + 700, startY + 440);
    pa2.moveTo (startX + 700, startY + 440);

    //绘制并设置下一处的坐标
    for (int i = 0; i < nextPath.size() ; i++) {
        setNextPos (nextPath[i]-1);
        pa.lineTo (endX + 700, endY + 440);
        pa2.addEllipse(endX + 695,endY + 435,15,15);
    }
    item->setPath (pa);
    item2->setPath (pa2);

}

void MainWindow::FindLoop ()
{
    if((startX!=-267 && modeComboBox->currentIndex ()!=2)||(modeComboBox->currentIndex ()==2&&Valid==false)){
        ui->textBrowser->setTextColor(QColor(255,0,0));
        strPrint.sprintf("起点已变更，请使用自定模式！");
        ui->textBrowser->append(strPrint);
        ui->textBrowser->setTextColor(QColor(0,0,0));
        return;
    }
    if(counti!=0){
        ui->textBrowser->setTextColor(QColor(255,0,0));
        strPrint.sprintf("导航自动中止.");
        ui->textBrowser->append(strPrint);
        ui->textBrowser->setTextColor(QColor(0,0,0));
        counti=0;
    }

    int trafficC=trafficComboBox->currentIndex ()+1,timeC=0;

    if(modeComboBox->currentIndex ()!=1){
        Backtracking(2,2,-1,9999,1);
        if(bestlen==MAX){
            FindPath(::record[n-1]);
            ui->textBrowser->setTextColor(QColor(255,0,0));
            strPrint.sprintf("原线路不可达，已生成近似路线.[仅供参考]");
            ui->textBrowser->append(strPrint);
            ui->textBrowser->setTextColor(QColor(0,0,0));
            return;
        }
    }
    else{
        Clear2 ();
        Backtracking(2,spindex,caldiff(starth,startmin,earlyh,earlymin),caldiff(starth,startmin,lateh,latemin),trafficComboBox->currentIndex ()+1);
        if(bestlen==MAX){
            if(trafficComboBox->currentIndex ()!= 1){
                //尝试更换出行方式
                Backtracking(2,spindex,caldiff(starth,startmin,earlyh,earlymin),caldiff(starth,startmin,lateh,latemin),2);
                trafficC = 2;
            }
            if(bestlen==MAX){
                //继续尝试提前时间
                Backtracking(2,spindex,caldiff(starth-1,startmin,earlyh,earlymin),caldiff(starth-1,startmin,lateh,latemin),2);
                timeC = 60;
            }
            if(bestlen==MAX){
                strPrint.sprintf("生成方案失败,请修改出行条件后重试.");
                ui->textBrowser->append(strPrint);
                return;
            }
        }
    }
    nextPath = mapbjut->getpath_loop ();

    strPrint.sprintf("搜索结果.");
    ui->textBrowser->append(strPrint);
    for (i = 1; i <= n; i++) {
        sprintf(tempstring, "%d.", record[bestvex[i] - 1]);
        astring=tempstring;
        astring.append(codec->toUnicode(sites[record[bestvex[i] - 1]].name));
        astring.append(" to");
        ui->textBrowser->setTextColor(QColor(0,0,255));
        ui->textBrowser->append(astring);
        ui->textBrowser->setTextColor(QColor(0,0,0));
        if (i == n) {
            stayminlocal = sites[record[bestvex[1]-1]].time;
            calcost(traffic, mgraph.arcs[bestvex[i]][bestvex[1]]);
        }
        else{
            calcost(trafficComboBox->currentIndex ()+1, mgraph.arcs[bestvex[i]][bestvex[i + 1]]);
            stayminlocal = sites[record[bestvex[i+1]-1]].time;
        }
        switch (trafficC) {
        case 1:
            strPrint.sprintf("步行耗时%.2lfmin",costmin);
            ui->textBrowser->append(strPrint);
            break;
        case 2:
            strPrint.sprintf("骑车耗时%.2lfmin",costmin);
            ui->textBrowser->append(strPrint);
            break;
        case 3:
            strPrint.sprintf("汽车耗时%.2lfmin",costmin);
            ui->textBrowser->append(strPrint);
            break;
        default:
            break;
        }
        strPrint.sprintf("所在地耗时:%d min", stayminlocal);
        ui->textBrowser->append(strPrint);
        strPrint.sprintf("出发时间: %d:%d", starth, startmin);
        ui->textBrowser->append(strPrint);
        starth = calh(starth, startmin, ceil(costmin));
        startmin = calmin(startmin, ceil(costmin));
        strPrint.sprintf("到达时间: %d:%d", starth, startmin);
        ui->textBrowser->append(strPrint);
        starth = calh(starth, startmin, stayminlocal);
        startmin = calmin(startmin, stayminlocal);
        strPrint.sprintf("离开时间: %d:%d", starth, startmin);
        ui->textBrowser->append(strPrint);
    }
    if(trafficC != trafficComboBox->currentIndex ()+1){
        ui->textBrowser->setTextColor(QColor(255,0,0));
        strPrint.sprintf("原出行方式不可达，已自动将出行方式切换为[骑车]");
        ui->textBrowser->append(strPrint);
        ui->textBrowser->setTextColor(QColor(0,0,0));
    }
    if(timeC != 0){
        ui->textBrowser->setTextColor(QColor(255,0,0));
        strPrint.sprintf("原出发时间不可达，已自动将出发时间提前[1小时]");
        ui->textBrowser->append(strPrint);
        ui->textBrowser->setTextColor(QColor(0,0,0));
    }

    //绘制路线
    Clear ();
    QGraphicsPathItem *item = new QGraphicsPathItem();
    QGraphicsPathItem *item2 = new QGraphicsPathItem();

    QPen pen,pen2;
    pen.setWidth (4);
    pen.setColor (Qt::red);
    item->setPen (pen);
    item->setFlag (QGraphicsItem::ItemIsPanel);
    pen2.setWidth (8);
    pen2.setColor (Qt::blue);
    item2->setPen (pen2);
    item2->setBrush(Qt::blue);
    item2->setFlag (QGraphicsItem::ItemIsPanel);

    for (int i = 0; i < nextPath.size (); i++) {
        qDebug() << nextPath[i]+1 << ", ";
    }
    scene->addItem (item);
    scene->addItem (item2);

    QPainterPath pa,pa2;               //path
    pa.moveTo (startX + 700, startY + 440);
    pa2.moveTo (startX + 700, startY + 440);

    for (int i = 0; i < nextPath.size() ; i++) {
        setNextPos (nextPath[i]);
        pa.lineTo (endX + 700, endY + 440);
        pa2.addEllipse(endX + 695,endY + 435,15,15);     //向path中添加图形
    }
    item->setPath (pa);
    item2->setPath (pa2);
}

void MainWindow::NewPath ()
{
    if((startX!=-267 && modeComboBox->currentIndex ()!=2)||(modeComboBox->currentIndex ()==2&&Valid==false)){
        ui->textBrowser->setTextColor(QColor(255,0,0));
        strPrint.sprintf("起点已变更，请使用自定模式！");
        ui->textBrowser->append(strPrint);
        ui->textBrowser->setTextColor(QColor(0,0,0));
        return;
    }
    if(counti!=0){
        ui->textBrowser->setTextColor(QColor(255,0,0));
        strPrint.sprintf("导航已开始！");
        ui->textBrowser->append(strPrint);
        ui->textBrowser->setTextColor(QColor(0,0,0));
        return;
    }
    QVector<QPoint> v;
    if(modeComboBox->currentIndex ()!=1){
        Backtracking(2,2,-1,9999,1);
        if(bestlen==MAX){
            strPrint.sprintf("路径不可达，请尝试其他地点.");
            ui->textBrowser->append(strPrint);
            return;
        }
    }
    else{
        Backtracking(2,spindex,caldiff(starth,startmin,earlyh,earlymin),caldiff(starth,startmin,lateh,latemin),trafficComboBox->currentIndex ()+1);
        if(bestlen==MAX){
            strPrint.sprintf("路径不可达！很遗憾您要迟到了.");
            ui->textBrowser->append(strPrint);
            return;
        }
    }
    nextPath = mapbjut->getpath_loop ();

    ui->textBrowser->setTextColor(QColor(255,0,0));
    strPrint.sprintf("分步导航结果");
    ui->textBrowser->append(strPrint);
    ui->textBrowser->setTextColor(QColor(0,0,0));
    sprintf(tempstring, "%d.", record[bestvex[1] - 1]);
    astring=tempstring;
    astring.append(codec->toUnicode(sites[record[bestvex[1] - 1]].name));
    astring.append(" to");
    ui->textBrowser->setTextColor(QColor(0,0,255));
    ui->textBrowser->append(astring);
    ui->textBrowser->setTextColor(QColor(0,0,0));

    calcost(trafficComboBox->currentIndex ()+1, mgraph.arcs[bestvex[1]][bestvex[2]]);
    stayminlocal = sites[record[bestvex[2]-1]].time;
    switch (trafficComboBox->currentIndex ()+1) {
    case 1:
        strPrint.sprintf("步行耗时%.2lfmin",costmin);
        ui->textBrowser->append(strPrint);
        break;
    case 2:
        strPrint.sprintf("骑车耗时%.2lfmin",costmin);
        ui->textBrowser->append(strPrint);
        break;
    case 3:
        strPrint.sprintf("汽车耗时%.2lfmin",costmin);
        ui->textBrowser->append(strPrint);
        break;
    default:
        break;
    }
    strPrint.sprintf("所在地耗时:%d min", stayminlocal);
    ui->textBrowser->append(strPrint);
    strPrint.sprintf("出发时间: %d:%d", starth, startmin);
    ui->textBrowser->append(strPrint);
    starth = calh(starth, startmin, ceil(costmin));
    startmin = calmin(startmin, ceil(costmin));
    strPrint.sprintf("到达时间: %d:%d", starth, startmin);
    ui->textBrowser->append(strPrint);
    starth = calh(starth, startmin, stayminlocal);
    startmin = calmin(startmin, stayminlocal);
    strPrint.sprintf("离开时间: %d:%d", starth, startmin);
    ui->textBrowser->append(strPrint);

    counti=1;

    Clear ();
    QGraphicsPathItem *item = new QGraphicsPathItem();
    QGraphicsPathItem *item2 = new QGraphicsPathItem();

    QPen pen,pen2;
    pen.setWidth (4);
    pen.setColor (Qt::red);
    item->setPen (pen);
    item->setFlag (QGraphicsItem::ItemIsPanel);
    pen2.setWidth (8);
    pen2.setColor (Qt::blue);
    item2->setPen (pen2);
    item2->setBrush(Qt::blue);
    item2->setFlag (QGraphicsItem::ItemIsPanel);

    qDebug() << nextPath[0]+1 << ", ";
    scene->addItem (item);
    scene->addItem (item2);

    QPainterPath pa,pa2;               //path

    pa.moveTo (startX + 700, startY + 440);
    pa2.moveTo (startX + 700, startY + 440);
    setNextPos (nextPath[0]);
    pa.lineTo (endX + 700, endY + 440);
    pa2.addEllipse(endX + 695,endY + 435,15,15);     //向path中添加图形
    item->setPath (pa);
    item2->setPath (pa2);

    core = new AnimationItem(1);
    QGraphicsItemAnimation *animt = new QGraphicsItemAnimation;
    animt->setItem(core);
    QTimeLine *timeLinetemp = new QTimeLine(2000);
    timeLinetemp->setCurveShape(QTimeLine::EaseOutCurve);
    timeLinetemp->setLoopCount(1);
    animt->setTimeLine(timeLinetemp);
    for (int i=0; i<200; i++)
    {
        this->getPos(startX+700,startY+440,endX+700,endY+440,i+1);
        animt->setPosAt(i/330.0, QPointF(this->x_ani,this->y_ani));
    }
    timeLinetemp->start();
    scene->addItem(core);
}

void MainWindow::getPos(int sX, int sY, int eX, int eY, int count)
{
    this->x_ani=sX+((eX-sX)/200.0)*count;
    this->y_ani=sY+((eY-sY)/200.0)*count;
}

void MainWindow::ContinuePath ()
{
    if(counti==0){
        ui->textBrowser->setTextColor(QColor(255,0,0));
        strPrint.sprintf("导航尚未开始！");
        ui->textBrowser->append(strPrint);
        ui->textBrowser->setTextColor(QColor(0,0,0));
        return;
    }
    scene->removeItem(core);
    QVector<QPoint> v;

    if(modeComboBox->currentIndex ()!=1){
        Backtracking(2,2,-1,9999,1);
        if(bestlen==MAX){
            return;
        }
    }
    else{
        Backtracking(2,spindex,caldiff(starth,startmin,earlyh,earlymin),caldiff(starth,startmin,lateh,latemin),trafficComboBox->currentIndex ()+1);
        if(bestlen==MAX){
            return;
        }
    }
    nextPath = mapbjut->getpath_loop ();

    strPrint.sprintf("搜索结果.");
    ui->textBrowser->append(strPrint);
    sprintf(tempstring, "%d.", record[bestvex[counti + 1] - 1]);
    astring=tempstring;
    astring.append(codec->toUnicode(sites[record[bestvex[counti + 1] - 1]].name));
    astring.append(" to");
    ui->textBrowser->setTextColor(QColor(0,0,255));
    ui->textBrowser->append(astring);
    ui->textBrowser->setTextColor(QColor(0,0,0));

    calcost(trafficComboBox->currentIndex ()+1, mgraph.arcs[bestvex[1]][bestvex[2]]);
    stayminlocal = sites[record[bestvex[2]-1]].time;
    if (counti == 9) {
        stayminlocal = sites[record[bestvex[1]-1]].time;
        calcost(traffic, mgraph.arcs[bestvex[i]][bestvex[1]]);
    }
    else{
        calcost(trafficComboBox->currentIndex ()+1, mgraph.arcs[bestvex[counti]][bestvex[counti + 1]]);
        stayminlocal = sites[record[bestvex[counti+1]-1]].time;
    }
    switch (trafficComboBox->currentIndex ()+1) {
    case 1:
        strPrint.sprintf("步行耗时%.2lfmin",costmin);
        ui->textBrowser->append(strPrint);
        break;
    case 2:
        strPrint.sprintf("骑车耗时%.2lfmin",costmin);
        ui->textBrowser->append(strPrint);
        break;
    case 3:
        strPrint.sprintf("汽车耗时%.2lfmin",costmin);
        ui->textBrowser->append(strPrint);
        break;
    default:
        break;
    }
    strPrint.sprintf("所在地耗时:%d min", stayminlocal);
    ui->textBrowser->append(strPrint);
    strPrint.sprintf("出发时间: %d:%d", starth, startmin);
    ui->textBrowser->append(strPrint);
    starth = calh(starth, startmin, ceil(costmin));
    startmin = calmin(startmin, ceil(costmin));
    strPrint.sprintf("到达时间: %d:%d", starth, startmin);
    ui->textBrowser->append(strPrint);
    starth = calh(starth, startmin, stayminlocal);
    startmin = calmin(startmin, stayminlocal);
    strPrint.sprintf("离开时间: %d:%d", starth, startmin);
    ui->textBrowser->append(strPrint);

    QGraphicsPathItem *item = new QGraphicsPathItem();
    QGraphicsPathItem *item2 = new QGraphicsPathItem();

    QPen pen,pen2;
    pen.setWidth (4);
    pen.setColor (Qt::red);
    item->setPen (pen);
    item->setFlag (QGraphicsItem::ItemIsPanel);
    pen2.setWidth (8);
    pen2.setColor (Qt::blue);
    item2->setPen (pen2);
    item2->setBrush(Qt::blue);
    item2->setFlag (QGraphicsItem::ItemIsPanel);

    qDebug() << nextPath[counti]+1 << ", ";
    scene->addItem (item);
    scene->addItem (item2);

    QPainterPath pa,pa2;               //path

    int tempsX,tempsY;
    for (int i = 1; i <= counti ; i++) {
        if(i==counti){
            tempsX=endX;
            tempsY=endY;
        }
        pa.moveTo (endX + 700, endY + 440);
        pa2.moveTo (endX + 700, endY + 440);
        setNextPos (nextPath[i]);
    }
    pa.lineTo (endX + 700, endY + 440);
    pa2.addEllipse(endX + 695,endY + 435,15,15);     //向path中添加图形
    item->setPath (pa);
    item2->setPath (pa2);

    core = new AnimationItem(1);
    QGraphicsItemAnimation *animt = new QGraphicsItemAnimation;
    animt->setItem(core);
    QTimeLine *timeLinetemp = new QTimeLine(2000);
    timeLinetemp->setCurveShape(QTimeLine::EaseOutCurve);
    timeLinetemp->setLoopCount(1);
    animt->setTimeLine(timeLinetemp);
    for (int i=0; i<200; i++)
    {
        this->getPos(tempsX+700,tempsY+440,endX+700,endY+440,i+1);
        animt->setPosAt(i/330.0, QPointF(this->x_ani,this->y_ani));
    }
    timeLinetemp->start();
    scene->addItem(core);

    counti++;
    if(counti==n){
        counti=0;
        ui->textBrowser->setTextColor(QColor(255,0,0));
        strPrint.sprintf("导航结束.");
        ui->textBrowser->append(strPrint);
        ui->textBrowser->setTextColor(QColor(0,0,0));
    }

}

void MainWindow::OpenPage()
{
    if(InnComboBox->currentIndex()==0)
        QDesktopServices::openUrl(QUrl("http://www.nerago.com/sys/hp_order.do?hotelCode=SOHOTO7836"));
    else if(InnComboBox->currentIndex()==1)
        QDesktopServices::openUrl(QUrl("http://www.nerago.com/sys/hp_order.do?hotelCode=SOHOTO7836"));
    else if(InnComboBox->currentIndex()==2)
        QDesktopServices::openUrl(QUrl("http://www.nerago.com/sys/hp_order.do?hotelCode=SOHOTO7836"));
    strPrint.sprintf("操作成功.");
    ui->textBrowser->append(strPrint);
}

void MainWindow::ChoosePlace()
{
    init(1);
    record[0] = startComboBox->currentIndex ()+1;
    n=1;
    Valid=true;

    int nCount = pListWidget->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = pListWidget->item(i);
        QWidget *pWidget = pListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            addRecord(i+1,tempn);
            n++;
        }
    }
    for (i = 1; i <= n; i++){
        for (j = 1; j <= n; j++){
            mgraph.arcs[i][j] = tempg[record[i - 1]][record[j - 1]];
            cout << mgraph.arcs[i][j] << " ";
        }
        cout << endl;
    }
    strPrint.sprintf("应用成功.");
    ui->textBrowser->append(strPrint);
}

void MainWindow::Clear ()
{
    QList<QGraphicsItem*> listItem = scene->items ();
    while (!listItem.empty ())
    {
        scene->removeItem (listItem.at (0));
        listItem.removeAt (0);
    }
    QGraphicsPixmapItem *item =
            scene->addPixmap (QPixmap("BJUTfinal.jpg"));
    item->setPos (-500, -250);
    QGraphicsPixmapItem *star =
            scene->addPixmap (QPixmap("star.png"));
    mark = scene->addPixmap (QPixmap("images/animation.png"));
    mark->setFlag(QGraphicsItem::ItemIsMovable);
    mark->setPos (-480, -200);
    AnimationItem *newstar = new AnimationItem;
    QGraphicsItemAnimation *anim = new QGraphicsItemAnimation;
    anim->setItem(newstar);
    QTimeLine *timeLine = new QTimeLine(4000);
    timeLine->setCurveShape(QTimeLine::SineCurve);
    timeLine->setLoopCount(0);
    anim->setTimeLine(timeLine);
    int y = 485;
    for (int i=0; i<400; i++)
    {
        anim->setPosAt(i/330.0, QPointF(i-430,y));
    }
    timeLine->start();
    scene->addItem(newstar);

    setStartStation ();
    star->setPos (startX+702, startY+408);
}

void MainWindow::Clear2 ()
{
    Valid=false;
    ui->textBrowser->setTextColor(QColor(255,0,0));
    ui->textBrowser->setText(tr(" 信息窗口重置成功 "));
    ui->textBrowser->setTextColor(QColor(0,0,0));
    strPrint.sprintf("当前时刻: %d:%d", temph, tempmin);
    ui->textBrowser->append(strPrint);
    QList<QGraphicsItem*> listItem = scene->items ();
    while (!listItem.empty ())
    {
        scene->removeItem (listItem.at (0));
        listItem.removeAt (0);
    }
    QGraphicsPixmapItem *item =
            scene->addPixmap (QPixmap("BJUTfinal.jpg"));
    item->setPos (-500, -250);
    QGraphicsPixmapItem *star =
            scene->addPixmap (QPixmap("star.png"));
    mark = scene->addPixmap (QPixmap("images/animation.png"));
    mark->setFlag(QGraphicsItem::ItemIsMovable);
    mark->setPos (-480, -200);
    AnimationItem *newstar = new AnimationItem;
    QGraphicsItemAnimation *anim = new QGraphicsItemAnimation;
    anim->setItem(newstar);
    QTimeLine *timeLine = new QTimeLine(4000);
    timeLine->setCurveShape(QTimeLine::SineCurve);
    timeLine->setLoopCount(0);
    anim->setTimeLine(timeLine);
    int y = 485;
    for (int i=0; i<400; i++)
    {
        anim->setPosAt(i/330.0, QPointF(i-430,y));
    }
    timeLine->start();
    scene->addItem(newstar);

    setStartStation ();
    star->setPos (startX+702, startY+408);
    init();
    if(counti!=0){
        ui->textBrowser->setTextColor(QColor(255,0,0));
        strPrint.sprintf("导航中止");
        ui->textBrowser->append(strPrint);
        ui->textBrowser->setTextColor(QColor(0,0,0));
        counti=0;
        return;
    }
}

void MainWindow::mouseDoubleClickEvent (QMouseEvent *e)         //鼠标事件
{
    if ( (e->x () >= 1223-50 && e->x () <= 1223 + 50) &&//x-110，y+50
         (e->y () >= 272 - 40 && e->y () <= 272 + 40 ))
    {
        strPath = "images//1";
        pic1.load (strPath);
        astring="                                             ";
        astring.append(codec->toUnicode(sites[1].name));
        location dialog(astring,pic1,1);
        dialog.setWindowTitle ("地点详情");
        dialog.exec();
    }
    else if ((e->x () >= 1071-40 && e->x () <= 1071 + 40) &&
             (e->y () >= 310 - 40 && e->y () <= 310 + 40 ) ) {
        strPath = "images//2";
        pic2.load (strPath);
        astring="                                             ";
        astring.append(codec->toUnicode(sites[2].name));
        location dialog(astring,pic2,2);
        dialog.setWindowTitle ("地点详情");
        dialog.exec();
    }
    else if ((e->x () >= 1111-50 && e->x () <= 1111 + 50) &&
             (e->y () >= 526 - 40 && e->y () <= 526 + 40 ) ) {
        strPath = "images//3";
        pic3.load (strPath);
        astring="                                             ";
        astring.append(codec->toUnicode(sites[3].name));
        location dialog(astring,pic3,3);
        dialog.setWindowTitle ("地点详情");
        dialog.exec();
    }
    else if ((e->x () >= 901-50 && e->x () <= 901 + 50) &&
             (e->y () >= 608 - 40 && e->y () <= 608 + 40 ) ) {
        strPath = "images//4";
        pic4.load (strPath);
        astring="                                             ";
        astring.append(codec->toUnicode(sites[4].name));
        location dialog(astring,pic4,4);
        dialog.setWindowTitle ("地点详情");
        dialog.exec();
    }
    else if ((e->x () >= 1076-50 && e->x () <= 1076 + 50) &&
             (e->y () >= 685 - 40 && e->y () <= 685 + 40 ) ) {
        strPath = "images//5";
        pic5.load (strPath);
        astring="                                             ";
        astring.append(codec->toUnicode(sites[5].name));
        location dialog(astring,pic5,5);
        dialog.setWindowTitle ("地点详情");
        dialog.exec();
    }
    else if ((e->x () >= 1302-200 && e->x () <= 1302+200) &&
             (e->y () >= 794 - 200 && e->y () <= 794 + 200 ) ) {
        strPath = "images//6";
        pic6.load (strPath);
        astring="                                             ";
        astring.append(codec->toUnicode(sites[6].name));
        location dialog(astring,pic6,6);
        dialog.setWindowTitle ("地点详情");
        dialog.exec();
    }
    else if ((e->x () >= 1511-200 && e->x () <= 1511+200) &&
             (e->y () >= 795 - 500 && e->y () <= 795 + 500 ) ) {
        strPath = "images//7";
        pic7.load (strPath);
        astring="                                             ";
        astring.append(codec->toUnicode(sites[7].name));
        location dialog(astring,pic7,7);
        dialog.setWindowTitle ("地点详情");
        dialog.exec();
    }
    else if ((e->x () >= 1178 - 50 && e->x () <= 1178 + 50) &&
             (e->y () >= 686 - 50 && e->y () <= 686 + 50 ) ) {
        strPath = "images//8";
        pic8.load (strPath);
        astring="                                             ";
        astring.append(codec->toUnicode(sites[8].name));
        location dialog(astring,pic8,8);
        dialog.setWindowTitle ("地点详情");
        dialog.exec();
    }
    else if ((e->x () >= 985 - 50 && e->x () <= 985 + 50) &&
             (e->y () >= 272 - 50 && e->y () <= 272 + 50 ) ) {
        strPath = "images//9";
        pic9.load (strPath);
        astring="                                             ";
        astring.append(codec->toUnicode(sites[9].name));
        location dialog(astring,pic9,9);
        dialog.setWindowTitle ("地点详情");
        dialog.exec();
    }
    else if ((e->x () >= 1177-50 && e->x () <= 1177+50) &&
             (e->y () >= 171 - 40 && e->y () <= 171 + 40 ) ) {
        strPath = "images//10";
        pic10.load (strPath);
        astring="                                             ";
        astring.append(codec->toUnicode(sites[10].name));
        location dialog(astring,pic10,10);
        dialog.setWindowTitle ("地点详情");
        dialog.exec();
    }
    else if ((e->x () >= 870-50 && e->x () <= 870 + 50) &&
             (e->y () >= 794 - 200 && e->y () <= 794 + 200 ) ) {
        ShowInnDialog();
    }
}

void MainWindow::ShowPlaceDialog ()
{
    init();
    if(modeComboBox->currentIndex ()!=2)
        return;
    QDialog *chooseDlg = new QDialog;
    QLabel *chooseLabel = new QLabel(tr("选择地点:"));
    QPushButton *SureBtn = new QPushButton(tr("应用"));
    QPushButton *CancelBtn = new QPushButton(tr("取消"));

    pListWidget = new QListWidget(this);
    pLineEdit = new QLineEdit(this);
    for (int i = 1; i <= tempn; ++i)
    {
        QListWidgetItem *pItem = new QListWidgetItem(pListWidget);
        pListWidget->addItem(pItem);
        pItem->setData(Qt::UserRole, i);
        QCheckBox *pCheckBox = new QCheckBox(this);
        astring=codec->toUnicode(sites[i].name);
        pCheckBox->setText(astring);
        pListWidget->addItem(pItem);
        pListWidget->setItemWidget(pItem, pCheckBox);
        connect(pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    }
    comboBox = new QComboBox(this);
    comboBox->setModel(pListWidget->model());
    comboBox->setView(pListWidget);
    comboBox->setLineEdit(pLineEdit);
    pLineEdit->setReadOnly(true);
    connect(pLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));

    connect (SureBtn, SIGNAL(clicked(bool)), this, SLOT(ChoosePlace()));
    connect (CancelBtn, SIGNAL(clicked(bool)), chooseDlg, SLOT(close ()));

    QGridLayout *mainlayout = new QGridLayout(chooseDlg);
    mainlayout->addWidget (chooseLabel, 0, 0);
    mainlayout->addWidget (comboBox, 0, 1);
    mainlayout->addWidget (SureBtn, 1, 1, 1, 1);
    mainlayout->addWidget (CancelBtn, 1, 2, 1, 1);

    chooseDlg->setWindowTitle (tr("自定行程"));
    chooseDlg->show ();
}

void MainWindow::ShowInnDialog ()
{
    QDialog *InnDlg = new QDialog;
    QLabel *picLabel = new QLabel();
    QTextBrowser *showtext = new QTextBrowser();
    QLabel *InnLabel = new QLabel(tr("选择房型:"));
    QPushButton *SureBtn2 = new QPushButton(tr("马上预订"));
    QPushButton *CancelBtn2 = new QPushButton(tr("取消订单"));

    strPath = "images//Inn";
    pic1.load (strPath);
    picLabel->setPixmap(pic1);
    strPrint.sprintf("   北京工大建国饭店座落在北京工业大学校园内,离北京奥运会羽毛球及艺术体操比赛场馆步行仅几分钟.酒店会给宾客带来别具一格的享受.");
    showtext->append(strPrint);
    strPrint.sprintf("   大堂环境幽雅,大堂酒吧各类世界名酒,专业SPA与健身房,是宾客商务旅行、休闲娱乐的理想之所.现在订购还可享受优惠.");
    showtext->append(strPrint);

    InnComboBox = new QComboBox;
    InnComboBox->addItem (tr("大床套间"));  //0
    InnComboBox->addItem (tr("商务标间"));  //1
    InnComboBox->addItem (tr("商务套房"));  //2

    connect (SureBtn2, SIGNAL(clicked(bool)), this, SLOT(OpenPage()));
    connect (CancelBtn2, SIGNAL(clicked(bool)), InnDlg, SLOT(close ()));

    QGridLayout *mainlayout = new QGridLayout(InnDlg);
    mainlayout->addWidget (picLabel, 0, 0, 1, 2);
    mainlayout->addWidget (showtext, 1, 0, 1, 2);
    mainlayout->addWidget (InnLabel, 2, 0);
    mainlayout->addWidget (InnComboBox, 2, 1);
    mainlayout->addWidget (SureBtn2, 3, 0, 1, 1);
    mainlayout->addWidget (CancelBtn2, 3, 1, 1, 1);

    InnDlg->setWindowTitle (tr("住宿预订"));
    InnDlg->show ();
}

void MainWindow::stateChanged(int state)
{
    bSelected = true;
    QString strSelectedData("");
    strSelectedText.clear();
    QCheckBox *pSenderCheckBox;
    int nCount = pListWidget->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = pListWidget->item(i);
        QWidget *pWidget = pListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            strSelectedData.append(strText).append(";");
        }
        //所点击的复选框
        if (pSenderCheckBox == pCheckBox)
        {
            int nData = pItem->data(Qt::UserRole).toInt();
            qDebug() << QString("I am sender...id : %1").arg(nData);
        }
    }
    if (strSelectedData.endsWith(";"))
        strSelectedData.remove(strSelectedData.count() - 1, 1);
    if (!strSelectedData.isEmpty())
    {
        strSelectedText = strSelectedData;
        pLineEdit->setText(strSelectedData);
        pLineEdit->setToolTip(strSelectedData);
    }
    else
    {
        pLineEdit->clear();
    }
    bSelected = false;
}

void MainWindow::textChanged(const QString &text)
{
    if (!bSelected)
        pLineEdit->setText(strSelectedText);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    __super::paintEvent(event);
    QPainter painter(this);
    painter.drawText(0, 30, m_showText.mid(m_curIndex));
    painter.drawText(width() - m_charWidth*m_curIndex, 30, m_showText.left(m_curIndex));
}

void MainWindow::updateIndex()
{
    update();
    m_curIndex++;
    if (m_curIndex*m_charWidth > width())
        m_curIndex = 0;
}
