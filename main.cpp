#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "QT版本：" << QT_VERSION_STR; // 验证QT6.8.3

    MainWindow w;
    w.setWindowTitle("QT6.8.3 音乐播放器");
    w.resize(847, 558);
    w.show();

    return a.exec();
}
