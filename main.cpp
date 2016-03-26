#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << QLocale::system().name();

    QTranslator translations;
    translations.load(":/whsim_" + QLocale::system().name());
    a.installTranslator(&translations);

    MainWindow w;
    w.show();

    return a.exec();
}
