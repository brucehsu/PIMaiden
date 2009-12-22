#include <QtGui/QApplication>
#include "webbrowser.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WebBrowser w;
    w.show();
    return a.exec();
}
