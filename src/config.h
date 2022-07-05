#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cerrno>

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
        m_filename = getCurrentDateTimeString();
    }

    void parseParameters(int argc, char **argv) {
        loadDefaults();

        // skip first param (exec path)
        for (int i = 1; i < argc; ++i) {
            std::string p = argv[i];
            auto delimiterIndex = p.find("=");

            if (delimiterIndex == std::string::npos) {
                printf("Unknown parameter: %s", p.c_str());
                exit(EINVAL);
            }

            auto key = p.substr(0, delimiterIndex);
            auto value = p.substr(delimiterIndex + 1);

            if (key.empty() || value.empty()) {
                printf("Invalid value: %s", p.c_str());
                exit(EINVAL);
            }

            if (key == "file") {
                m_filename = value;
            }
            if (key == "display") {
                m_displayIndex = std::stoi(value);
            }
            if (key == "x") {
                m_x = std::stoi(value);
            }
            if (key == "y") {
                m_y = std::stoi(value);
            }
            if (key == "w") {
                m_w = std::stoi(value);
            }
            if (key == "h") {
                m_h = std::stoi(value);
            }
        }
    }

    std::string getFileName(const std::string &format) const {
        auto name = m_filename;

        if (name.empty()) {
            name = getCurrentDateTimeString();
        }

        if (name.at(0) == '/') {
            return name + "." + format;
        }

        return "/var/tmp/" + name + "." + format;
    }
    int getDisplayIndex() const { return m_displayIndex; }
    int getX() const { return m_x; }
    int getY() const { return m_y; }
    int getW() const { return m_w; }
    int getH() const { return m_h; }

private:
    Config() {}
    Config(const Config&) {}

    static std::string getCurrentDateTimeString() {
        // get current time
        std::time_t t = std::time(0);
        std::tm *now = std::localtime(&t);

        char result[100];
        std::strftime(result, sizeof(result), "%Y_%m_%d-%H_%M_%S", now);

        return std::string(result);
    }

private:
    std::string m_filename {};
    int m_displayIndex {0};
    int m_x {0};
    int m_y {0};
    int m_w {-1};
    int m_h {-1};
};

#endif // CONFIG_H
