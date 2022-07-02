#include <QGuiApplication>
#include <QDebug>

#include "appmain.h"
#include "config.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Config::instance()->parseParameters(argc, argv);
    AppMain appmain;

    appmain.init();
    appmain.captureAndSave();

    return app.exec();
}
