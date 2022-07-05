#include "screenmanager.h"
#include "config.h"

int main(int argc, char *argv[])
{
    const auto config = Config::instance();
    config->parseParameters(argc, argv);

    Screen().captureScreen(
        config->getDisplayIndex(),
        config->getX(), config->getY(),
        config->getW(), config->getH());

    return 0;
}
