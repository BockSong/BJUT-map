#include "mainwindow.h"
#include <QFont>
#include <QDebug>
#include <QPixmap>
#include <QSplashScreen>
#include <QApplication>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap pixmap("welcome");    //设置启动图片
    QSplashScreen splash(pixmap); //利用QPixmap对象创建QSplashScreen对象
    splash.show ();               //显示启动图片
    a.processEvents ();           //使程序在显示图片时能响应其他事件
    QFont font("ARPL KaitiM GB", 12); //设置字体和大小
    font.setBold (true);
    a.setFont (font);
    MainWindow w;
    w.show();
    splash.finish (&w);           //主窗体初始化完成后，结束启动画面
    return a.exec();
}
