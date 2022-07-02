QT += core network
CONFIG += c++11
TARGET = betterss

DEFINES += APP_ON_TARGET
DESTDIR = $$(APP_INSTALL_IVI_BIN_DIR)

target.path = /opt/bin
INSTALLS += target

SOURCES += appmain.cpp \
            main.cpp \
            network/tcptransceiver.cpp \
            screenmanager.cpp

HEADERS += appmain.h \
    config.h \
    network/tcptransceiver.h \
    screenmanager.h

INCLUDEPATH+=E:/fw/framework/include
INCLUDEPATH+=E:/fw/QNX/qnx700/target/qnx7/usr/include
INCLUDEPATH+=E:/fw/QNX/qnx700/target/qnx7/usr/include/c++/v1

LIBS += -lscreen
