#ifndef APPMAIN_H
#define APPMAIN_H

#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include <QDir>
#include <QWidget>
#include <QDebug>
#include <QtWidgets>

class AppMain : public QWidget {
public:
    AppMain() {
        setWindowTitle("Screenshot");
        setWindowState(Qt::WindowMaximized);
    }

    void init() {
        // qDebug("shooting");
        // saveScreenshot(shootScreen());
        // qDebug("done");
        show();
    }

    void saveScreenshot(const QPixmap &image)
    {
        if (!image) {
            qCritical("Invalid image");
            return;
        }

        const auto path = "/var/tmp/bs.png";

        if (!image.save(path)) {
            qCritical("Could not save the image");
        }
    }

    QPixmap shootScreen(int screenIndex = 0, int x = 0, int y = 0, int w = -1, int h = -1) {
#if 1
        const auto screens = QGuiApplication::screens();

        if (screenIndex >= screens.length()) {
            qCritical("Could not get screen %d, screens: %d", screenIndex, screens.length());
            return QPixmap();
        }

        return screens.at(screenIndex)->grabWindow(0);
#else
        auto screen = QGuiApplication::primaryScreen();

        if (!screen) {
            qCritical("Could not get primary screen");
            return QPixmap();
        }

        auto geom = screen->geometry();
        qDebug() << screen->manufacturer() << screen->isWindowType();
        qDebug("%d %d %d %d", geom.x(), geom.y(), geom.width(), geom.height());
        return screen->grabWindow(0, geom.x(), geom.y(), geom.width(), geom.height());
#endif
    }
};


#endif // APPMAIN_H
