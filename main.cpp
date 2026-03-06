#include "mainwindow.h"

#include <QApplication>
#include <mainwindow.h>
#include <QDebug>
#include <net.h>
#include <qfile.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString qss;
    QFile qss_file(":/QSS/ubuntu.qss");
    qss_file.open(QFile::ReadOnly);
    qss = qss_file.readAll();
    qss_file.close();
    a.setStyleSheet(qss);

    MainWindow *m =new MainWindow();
    m->show();
    // if(!m->connectServer()){
    //     delete m;
    // }
    //qDebug() << "asd";
    int ret =a.exec();
    delete m;
    return ret;
    //return a.exec();
    //qDebug() << "asd";
}
