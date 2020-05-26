#include "widget.h"
#include <QApplication>
#include <qsplashscreen.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName( QString("Plasma Control") );
    Widget w;
    w.setWindowTitle("Plasma Control 1.0");

    w.show();

    return a.exec();
}
