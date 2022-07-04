#include "appmain.h"
#include "config.h"

int main(int argc, char *argv[])
{
    Config::instance()->parseParameters(argc, argv);
    AppMain appmain;

    appmain.init();
    appmain.captureAndSave();

    return 0;
}
