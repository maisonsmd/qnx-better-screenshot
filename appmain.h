#ifndef APPMAIN_H
#define APPMAIN_H

#include <QGuiApplication>
#include <QPixmap>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QQueue>
#include <QElapsedTimer>
#include <QThread>

#include "errno.h"

#include "screenmanager.h"
#include "config.h"
#include "network/tcptransceiver.h"

class AppMain : public QObject {
    Q_OBJECT

public:
    AppMain() {
        connect(this, &AppMain::quit, qApp, &QCoreApplication::quit, Qt::QueuedConnection);
    }

    void init() {
        // m_transceiver->init();
    }

    void captureAndSave() {
        QTimer::singleShot(0, this, [&](){
            QThread::currentThread()->setPriority(QThread::HighestPriority);
            QElapsedTimer timer;
            timer.start();

            auto config = Config::instance();

            saveImage(m_screen.captureScreen(
                          config->getDisplayIndex(),
                          config->getX(), config->getY(),
                          config->getW(), config->getH()
                          ),
                      config->getFileName());

            emit quit();
            qDebug() << timer.elapsed();
        });
    }

#if 1
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

signals:
    void quit();

public slots:
    void capture() {
        // m_imageId++;
        // m_imageQueue.push_back({m_imageId, m_screen.captureScreen(0, 0, 0, 100, 100)});
    }

private:
    Transceiver *m_transceiver {Transceiver::instance()};

    Screen m_screen {};
    QTimer m_timer {};

    size_t m_imageId {0};
    struct Image {
        size_t id;
        QPixmap image;
    };
    QQueue<Image> m_imageQueue {};
};


#endif // APPMAIN_H
