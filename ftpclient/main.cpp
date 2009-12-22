#include <QtGui/QApplication>
#include "ftpclient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FtpClient w;
    w.show();
    return a.exec();
}
