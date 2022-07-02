#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>
#include <QDateTime>

class Config {
public:
    static Config *instance() {
        static Config *_instance {nullptr};
        if (!_instance) {
            _instance = new Config();
        }
        return _instance;
    }

    void loadDefaults() {
        m_filename = QDateTime::currentDateTime().toString("yyyy_MM_dd-hh_mm_ss") + ".png";
    }

    void parseParameters(int argc, char **argv) {
        loadDefaults();

        // skip first param (exec path)
        for (int i = 1; i < argc; ++i) {
            QString p = argv[i];
            auto pair = p.split("=");

            if (pair.size() != 2) {
                qWarning() << "Unknow parameter: " << p;
                continue;
            }

            if (pair.first() == "filename") {
                m_filename = pair.last();
            }
            if (pair.first() == "display") {
                m_displayIndex = pair.last().toInt();
            }
            if (pair.first() == "x") {
                m_x = pair.last().toInt();
            }
            if (pair.first() == "y") {
                m_y = pair.last().toInt();
            }
            if (pair.first() == "w") {
                m_w = pair.last().toInt();
            }
            if (pair.first() == "h") {
                m_h = pair.last().toInt();
            }
        }
    }

    QString getFileName() const { return m_filename; }
    int getDisplayIndex() const { return m_displayIndex; }
    int getX() const { return m_x; }
    int getY() const { return m_y; }
    int getW() const { return m_w; }
    int getH() const { return m_h; }

private:
    Config() {}
    Config(const Config&) {}

private:
    QString m_filename {};
    int m_displayIndex {0};
    int m_x {0};
    int m_y {0};
    int m_w {-1};
    int m_h {-1};
};

#endif // CONFIG_H
