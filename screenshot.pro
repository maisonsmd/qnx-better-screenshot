QT += core
CONFIG += c++11
TARGET = betterss

DEFINES += APP_ON_TARGET
DESTDIR = $$(APP_INSTALL_IVI_BIN_DIR)

target.path = /opt/bin
INSTALLS += target

SOURCES += appmain.cpp \
            main.cpp \
            screenshot.cpp

HEADERS += appmain.h \
    screenshot.h

INCLUDEPATH+=E:/fw/framework/include
INCLUDEPATH+=E:/fw/QNX/qnx700/target/qnx7/usr/include
INCLUDEPATH+=E:/fw/QNX/qnx700/target/qnx7/usr/include/c++/v1

LIBS += -lscreen
