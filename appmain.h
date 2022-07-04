#ifndef APPMAIN_H
#define APPMAIN_H

#include "errno.h"

#include "screenmanager.h"
#include "config.h"

class AppMain {

public:
    AppMain() {
    }

    void init() {
    }

    void captureAndSave() {
        auto config = Config::instance();

        m_screen.captureScreen(
                      config->getDisplayIndex(),
                      config->getX(), config->getY(),
                      config->getW(), config->getH()
                      );
    }

#if 0
    void saveImage(const QPixmap &_image, const QString &_name)
    {
        if (!_image) {
            qCritical("Invalid image");
            return;
        }


        QString path = "/var/tmp/screenshot/";
        QDir dir(path);

        if (!dir.exists()) {
            dir.mkpath(path);
        }

        if (!_image.save(path + _name, "png", 50)) {
            qCritical("Failed to save the image, err %d(%s)", errno, strerror(errno));
        }
    }
#endif

private:
    Screen m_screen {};
};


#endif // APPMAIN_H
