#include <QGuiApplication>
#include "appmain.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    AppMain appmain;
    appmain.init();

    return app.exec();
}
