#include "mainwindow.h"

#include <QApplication>
#include <QProcess>
#include <QString>
#include <QStringList>
#include "Logger.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Logger::initLog();
    MainWindow w;
    w.show();


    return a.exec();
}
