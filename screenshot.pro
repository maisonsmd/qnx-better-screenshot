QT += gui widgets
CONFIG += c++11
TARGET = betterss

DEFINES += APP_ON_TARGET
DESTDIR = $$(APP_INSTALL_IVI_BIN_DIR)

target.path = /opt/bin
INSTALLS += target

SOURCES += appmain.cpp \
            main.cpp

HEADERS += appmain.h
