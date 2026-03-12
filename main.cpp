#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("音乐播放器");
    w.setWindowIcon(QIcon("musicicon.png"));
    w.resize(847, 558);
    w.show();

    return a.exec();
}
