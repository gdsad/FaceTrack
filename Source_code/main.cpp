#include "mainwindow.h"
#include <QApplication>
#include <QEvent>
#include <QGraphicsDropShadowEffect>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Set locale to en_US to use "." as the decimal separator.
    setlocale(LC_ALL, "en_US.UTF-8");

    // Agrego Fuentes Utilizadas   
    QFontDatabase::addApplicationFont(":/fonts/OpenSans.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Montserrat.otf");

    // Saco los bordes y hago transparente el redondeado
    w.setWindowFlag(Qt::FramelessWindowHint);
    w.setAttribute(Qt::WA_TranslucentBackground);

    w.show();

    return a.exec();
}


