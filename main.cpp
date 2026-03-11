#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("音乐播放器");
    w.resize(847, 558);
    w.show();

    return a.exec();
}
