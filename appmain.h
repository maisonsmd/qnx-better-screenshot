#ifndef APPMAIN_H
#define APPMAIN_H

#include <QImage>
#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QElapsedTimer>
#include <QThread>

#include "screenshot.h"

class AppMain : public QObject {
    Q_OBJECT

    QPixmap image;
    Screenshot ss;
    QTimer timer;

public:
    AppMain() {
        connect(&timer, &QTimer::timeout, this, &AppMain::capture);
    }

    void init() {
        timer.setInterval(100);
        timer.setSingleShot(true);
        timer.start();
    }

    void saveImage(const QPixmap &image)
    {
        if (!image) {
            qCritical("Invalid image");
            return;
        }

        qDebug() << image.size();

        const QString path = "/var/tmp/bs.png";

        if (!image.save(path)) {
            qCritical("Could not save the image");
        }
    }

public slots:
    void capture() {
        qDebug("capturing");

        QThread::currentThread()->setPriority(QThread::HighestPriority);
        QElapsedTimer e;

        e.start();
        image = ss.captureScreen(0, 0, 0, 100, 100);
        qDebug() << e.elapsed();

        e.restart();
        saveImage(image);
        qDebug() << e.elapsed();

        qDebug("done");
    }
};


#endif // APPMAIN_H
