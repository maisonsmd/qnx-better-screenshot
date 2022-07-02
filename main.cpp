#include <QApplication>
#include "appmain.h"

int main(int argc, char *argv[])
{
    qDebug("hello");

    QApplication app(argc, argv);

    AppMain appmain;
    appmain.init();

    qDebug("finished");

    return app.exec();
}
